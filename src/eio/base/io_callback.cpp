#include "io_callback.hpp"
#include <stdio.h>

namespace eio
{

	io_callback::io_callback(void** function/*=0*/,void* p/*=0*/,ebase::ref_class_i* holder)
	{
        if (function)this->_function.set(function, p,holder);
	}

	io_callback::~io_callback()
	{

	}


	void io_callback::invoke(io_request* request)
	{
        if(!this->_function.isset())return;
		_function.invoke(request);
	}


    io_callback_waitor::io_callback_waitor()
    {
    }
    io_callback_waitor::~io_callback_waitor()
    {
        request.reset(0);
    }
	void io_callback_waitor::on_callback(eio::io_request* request)
	{
		ebase::mutex_lock_scope l(&this->request);

		this->request.reset(request);
		ebase::event_signal::fire_event();
	}

	void io_callback_waitor::on_destroy()
	{

	}
    void io_callback_waitor::clear()
    {
        request.reset(0);
    }
};