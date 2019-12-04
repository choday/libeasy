#pragma once
#include "precompile.h"
#include "allocator.hpp"
#include "atomic.hpp"

namespace ebase
{
	class ref_class_i
	{
	public:
		virtual long		add_ref() = 0;
		virtual long		release() = 0;
		virtual long		ref_count() const = 0;
	private:
		virtual void		on_destroy() = 0;
	};

	template<class CLASS_TYPE=ref_class_i>
	class ref_class:public CLASS_TYPE,public class_allocator
	{
	public:
		virtual ~ref_class();
		ref_class(ref_class_i* outref = nullptr);

		
		virtual long		add_ref() override;
		virtual long		release() override;

		inline	void		set_out_ref(ref_class_i* outref){this->_out_ref=outref;}
		inline	ref_class_i*	get_out_ref() const {return this->_out_ref;}
		virtual long		ref_count() const override{if(_out_ref)return _out_ref->ref_count();else return _ref_count;}
	private://禁用下列函数
		ref_class(const ref_class&);
		ref_class& operator=(const ref_class& v);

		virtual void		on_destroy() override;
	protected:
		ref_class_i*			_out_ref;
		long volatile			_ref_count;
	};
 //template class __declspec(dllexport) ref_class<>;

	template<class CLASS_TYPE=ref_class_i>
	class ref_ptr
	{
	public:
		ref_ptr(CLASS_TYPE* ptr = nullptr);
		ref_ptr(const ref_ptr& ptr);
		~ref_ptr();

		void reset(CLASS_TYPE* ptr = nullptr);
		const ref_ptr& operator=(CLASS_TYPE* ptr);
		const ref_ptr& operator=(const ref_ptr& ptr);

		bool		operator==(CLASS_TYPE* ptr) const;
		bool		operator==(const ref_ptr& ptr) const;
		bool		operator!=(CLASS_TYPE* ptr) const;
		bool		operator!=(const ref_ptr& ptr) const;

		CLASS_TYPE*			get();
		CLASS_TYPE*	get() const;

		operator CLASS_TYPE*();
		operator CLASS_TYPE*() const;
		CLASS_TYPE&	operator*();
		CLASS_TYPE*	operator->();
        CLASS_TYPE*	operator->() const;
		bool			valid() const;
		CLASS_TYPE*		escape();
	private:
		mutable CLASS_TYPE*			_object;
	};

//////////////////////////////////ref_class//////////////////////////
	template<class CLASS_TYPE/*=ref_class_i*/>
	ebase::ref_class<CLASS_TYPE>::~ref_class(){}

	template<class CLASS_TYPE/*=ref_class_i*/>
	ebase::ref_class<CLASS_TYPE>::ref_class(ref_class_i* outref /*= nullptr*/):_out_ref(outref),_ref_count(0){}

	template<class CLASS_TYPE/*=ref_class_i*/>
	ebase::ref_class<CLASS_TYPE>::ref_class(const ref_class&):_out_ref(nullptr),_ref_count(0){}

	template<class CLASS_TYPE/*=ref_class_i*/>
	ref_class<CLASS_TYPE>& ebase::ref_class<CLASS_TYPE>::operator=(const ref_class& v){return *this;}

	template<class CLASS_TYPE/*=ref_class_i*/>
	long ebase::ref_class<CLASS_TYPE>::add_ref()
	{
		long result = atomic::increment(&_ref_count);
		if (_out_ref)_out_ref->add_ref();
		return result;
	}

	template<class CLASS_TYPE/*=ref_class_i*/>
	long ebase::ref_class<CLASS_TYPE>::release()
	{
		long result = atomic::decrement(&_ref_count);
		assert(result>=0);

		if (_out_ref)_out_ref->release();
		else if (0 == result)this->on_destroy();

		return result;
	}

	template<class CLASS_TYPE/*=ref_class_i*/>
	void ebase::ref_class<CLASS_TYPE>::on_destroy(){
		delete this;
	}

///////////////////ref_ptr////////////////////////

	template<class CLASS_TYPE>
	ebase::ref_ptr<CLASS_TYPE>::ref_ptr(CLASS_TYPE* ptr /*= nullptr*/) :_object(nullptr){reset(ptr);}

	template<class CLASS_TYPE>
	ebase::ref_ptr<CLASS_TYPE>::ref_ptr(const ref_ptr& ptr) :_object(nullptr){reset(ptr._object);}
	
	template<class CLASS_TYPE>
	ebase::ref_ptr<CLASS_TYPE>::~ref_ptr(){reset();}

	template<class CLASS_TYPE>
	CLASS_TYPE* ebase::ref_ptr<CLASS_TYPE>::get() {return _object;}

	template<class CLASS_TYPE>
	CLASS_TYPE* ebase::ref_ptr<CLASS_TYPE>::get() const {return _object;}

	template<class CLASS_TYPE>
	const ref_ptr<CLASS_TYPE>& ebase::ref_ptr<CLASS_TYPE>::operator=(CLASS_TYPE* ptr){reset(ptr);return *this;}

	template<class CLASS_TYPE>
	const ref_ptr<CLASS_TYPE>& ebase::ref_ptr<CLASS_TYPE>::operator=(const ref_ptr& ptr){reset(ptr._object);return *this;}

	template<class CLASS_TYPE>
	bool ebase::ref_ptr<CLASS_TYPE>::operator==(CLASS_TYPE* ptr) const {return _object == ptr;}

	template<class CLASS_TYPE>
	bool ebase::ref_ptr<CLASS_TYPE>::operator==(const ref_ptr& ptr) const {return _object == ptr._object;}

	template<class CLASS_TYPE>
	bool ebase::ref_ptr<CLASS_TYPE>::operator!=(CLASS_TYPE* ptr) const {return _object != ptr;}

	template<class CLASS_TYPE>
	bool ebase::ref_ptr<CLASS_TYPE>::operator!=(const ref_ptr& ptr) const {return _object != ptr._object;}

	template<class CLASS_TYPE>
	ebase::ref_ptr<CLASS_TYPE>::operator CLASS_TYPE*(){return get();}

	template<class CLASS_TYPE>
	CLASS_TYPE& ebase::ref_ptr<CLASS_TYPE>::operator*(){return *_object;}

	template<class CLASS_TYPE>
	CLASS_TYPE* ebase::ref_ptr<CLASS_TYPE>::operator->(){return get();};

    template<class CLASS_TYPE/*=ref_class_i*/>
    CLASS_TYPE* ebase::ref_ptr<CLASS_TYPE>::operator->() const {return get();}

	template<class CLASS_TYPE/*=ref_class_i*/>
	ebase::ref_ptr<CLASS_TYPE>::operator CLASS_TYPE*() const{return this->get();}

	template<class CLASS_TYPE>
	bool ebase::ref_ptr<CLASS_TYPE>::valid() const{return nullptr != get();}

	template<class CLASS_TYPE>
	CLASS_TYPE* ebase::ref_ptr<CLASS_TYPE>::escape()	{CLASS_TYPE* p = _object;_object = nullptr;return p;}

	template<class CLASS_TYPE>
	void ebase::ref_ptr<CLASS_TYPE>::reset(CLASS_TYPE* ptr /*= nullptr*/)
	{
		if (_object == ptr)return;

		CLASS_TYPE* older=_object;
		_object = ptr;
		if (ptr)((ref_class_i*)ptr)->add_ref();

		if (older != nullptr)((ref_class_i*)older)->release();
	}
};


