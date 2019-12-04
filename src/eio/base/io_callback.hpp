#pragma once
#include "../../ebase/ref_class.hpp"
#include "../../ebase/executor.hpp"
#include "../../ebase/ref_function.hpp"

namespace eio
{
	class io_request;
	class io_callback
	{
	public:
		io_callback(void** function=0,void* _object=0,ebase::ref_class_i* holder=0);
		~io_callback();



		template<typename class_name>
		static inline io_callback       bind( void (class_name::*function)(io_request* request) ,class_name* object){return io_callback((void**)&function,object,object);}
		static inline io_callback       bind( void (*function)(io_request* request) ){return io_callback((void**)&function,0,0);}
		
        template<typename class_name>
		inline void                     set_function( void (class_name::*function)(io_request* request) ,class_name* object){ this->_function.set((void**)&function,object,object);}
        inline void                     clear(){this->_function.clear();}
		       void	                    invoke(io_request* request);

	private:
        ebase::ref_function            _function;
	};

	class io_callback_waitor:public ebase::event_signal,public ebase::ref_class<>
	{
	public:
        io_callback_waitor();
        ~io_callback_waitor();

		inline io_callback		get_callback(){return io_callback::bind( &io_callback_waitor::on_callback,this);}
		inline operator		    io_callback(){return get_callback();}

		void                    on_callback(eio::io_request* request);
		
		inline eio::io_request*	wait_event(unsigned long timeout_ms=-1)
		{
			
			ebase::event_signal::wait_event(timeout_ms);
			return this->request.get();
		}

		virtual void		    on_destroy() override;
		
        void                    clear();

        ebase::mutex_lock_t<ebase::ref_ptr<eio::io_request>> request;
	private://²»ÔÊÐínew ²Ù×÷·û
		inline void* operator new(size_t cb){return class_allocator::operator new(cb);}
		inline void * operator new[]( size_t cb ){return class_allocator::operator new[](cb);}

	};

};