#include "name_resolver.hpp"
#include "eio.hpp"


namespace eio
{


	bool name_resolver::resolver(const ebase::string& name,const ebase::string& service_of_port,const io_callback& callback,ebase::executor* callback_executor)
	{
		ebase::ref_ptr<eio::name_resolver::request>	p = name_resolver::create_request();

		p->host = name;
		p->port = service_of_port;
		p->completed_callback = callback;
		p->completed_executor = callback_executor;
		
		return p->start();
	}

	ebase::ref_ptr<eio::name_resolver::request> name_resolver::create_request()
	{
		return  new name_resolver::request();
	}

	name_resolver::request::request():result(0),_current(0)
	{
		io_request::_request_executor = get_name_executor();
	}

	name_resolver::request::~request()
	{
		clear();
	}

    bool name_resolver::request::is_success()
    {
        return this->error.code ==0;
    }

    const void* name_resolver::request::move_begin()
    {
        _current = result;

        if(0==_current)return 0;
        return ((struct addrinfo*)_current)->ai_addr;
    }

    const void* name_resolver::request::move_next()
    {
        if(0==_current)return 0;
        _current=((struct addrinfo*)_current)->ai_next;
        if(0==_current)return 0;
        return ((struct addrinfo*)_current)->ai_addr;
    }

    const void* name_resolver::request::current()
    {
        if(0==_current)return 0;
        return ((struct addrinfo*)_current)->ai_addr;
    }

    const int name_resolver::request::current_size()
    {
        if(0==_current)return 0;
        return ((struct addrinfo*)_current)->ai_addrlen;
    }

    void name_resolver::request::internal_do_request()
	{
		int code;

		clear();

        code = ::getaddrinfo( this->host.c_str(),this->port.c_str(),0, (struct addrinfo**)&_current);

        if (code)
        {
#ifdef _WIN32
            this->error.set_system_error(code);
#else
            if (EAI_SYSTEM == code)
            {
                this->error.set_system_error(errno);
            }
            else
            {
                this->error.set_user_error(code, gai_strerror(code));
            }
            
#endif
            return;
        }

		(*(void**)&this->result) = this->_current;

		return;
	}

	void name_resolver::request::clear()
	{
		if(result)::freeaddrinfo((struct addrinfo*)result);
		(*(struct addrinfo**)&this->result) = 0;
		_current=0;
	}

};