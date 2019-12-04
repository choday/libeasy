#include "io_request.hpp"

namespace eio
{
#define flags_io_request_do_completed 1
#define flags_io_request_do_request 2


	io_request::io_request():completed_executor(0),_request_executor(0),_call_completed_later(false)
	{
		flags.reset(0);
	}

	io_request::~io_request()
	{
		assert(!this->is_schedule());
	}

	bool io_request::start()
	{
		if(flags.set(0))
		{
			assert(0=="io_request io pendding...");
			return false;
		}
		
		bool result = this->do_request();
		return result;
	}

	void io_request::do_completed()
	{
		if(this->completed_executor)
		{
			flags.set(flags_io_request_do_completed);
			this->completed_executor->post(this,true);
		}else 
		{
			this->completed_callback.invoke(this);
		}
	}

	bool io_request::do_request()
	{
		if(this->_request_executor)
		{
			flags.set(flags_io_request_do_request);
			this->_request_executor->post(this,true);
		}else 
		{
			this->internal_do_request();
			if(!this->_call_completed_later)this->do_completed();
		}
		return true;
	}

	void io_request::internal_do_request()
	{
		this->_request_callback.invoke(this);
	}

	void io_request::run()
	{
		if(flags.test(flags_io_request_do_request))
		{
			flags.clear(flags_io_request_do_request);
			this->internal_do_request();	
			if(!this->_call_completed_later)this->do_completed();

		}else if(flags.test(flags_io_request_do_completed))
		{
			flags.clear(flags_io_request_do_completed);
			this->completed_callback.invoke(this);	
		}else
		{
			assert(0=="no task...");
		}
	}

};