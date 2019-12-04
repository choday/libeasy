#pragma once
#include "ref_class.hpp"
#include "executor.hpp"
#include "runnable.hpp"
#include "ref_function.hpp"
//如果是带虚继承和类成员函数，请使用set_function_virtual_inheritance
namespace ebase
{
    //支持虚函数
	class event_emitter:public runnable
	{
	public:
		event_emitter();
		~event_emitter();

		inline void set_event_source(ref_class_i* event_source_and_out_ref){ebase::runnable::set_out_ref( event_source_and_out_ref );this->_event_source=event_source_and_out_ref;}
        inline void set_event_executor( executor* event_executor ){this->_event_executor = event_executor;}
		inline void set_event_source_and_executore(ref_class_i* event_source_and_out_ref,executor* event_executor)
		{
            ebase::runnable::set_out_ref( event_source_and_out_ref );
            this->_event_source=event_source_and_out_ref;
			this->_event_executor = event_executor;
		}

        typedef void (ref_class_i::*event_function)(ref_class_i* fire_from_handle);

		template<typename class_name>
		inline void                     set_function( void (class_name::*function)(ref_class_i* fire_from_handle),class_name* object)
        {
            this->_function.set((void**)&function,object,object);
        }
		void                            set_function(  void (*function)(ref_class_i* fire_from_handle) ){this->_function.set((void**)&function,0,0);}
		void                            clear();
		bool                            fire();

	protected:
		virtual void	                run();

        ref_function        _function;
		ref_class_i*		_event_source;
		executor*			_event_executor;
	};

	class event_emitter_waitor:public event_signal,public ref_class<>
	{
	public:
		event_emitter_waitor();
		~event_emitter_waitor();

		inline void set_event(event_emitter& e){if(_event_emitter)_event_emitter->clear();_event_emitter=&e;e.set_function(&event_emitter_waitor::on_callback,this);}
		
		
		inline ref_class_i*	wait_event(unsigned long timeout_ms=-1)
		{
			//this->fire_from_handle.reset(0);
			
			bool result = event_signal::wait_event(timeout_ms);

			mutex_lock_scope l(&this->fire_from_handle);
			
			return this->fire_from_handle.get();
		}

		void                        clear();

		mutex_lock_t<ref_ptr<>>		fire_from_handle;

	private://不允许new 操作符
		event_emitter*		_event_emitter;

		void                on_callback(ref_class_i* fire_from_handle);

		virtual void		on_destroy();
		inline void* operator new(size_t cb){return class_allocator::operator new(cb);}
		inline void * operator new[]( size_t cb ){return class_allocator::operator new[](cb);}
	};
};