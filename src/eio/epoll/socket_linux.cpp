#include "socket_linux.hpp"
#include "epoll_linux.hpp"
#include <stdio.h>
namespace eio
{

	socket_linux::socket_linux(ebase::executor* event_executor/*=0*/):socket(event_executor)
	{

	}

	bool socket_linux::enter_event_listener()
	{
		if(epoll_linux::instance()->add_handle( (int)this->get_handle(),this ) )
		{
			this->add_ref();
			return true;
		}
		return false;
	}

	void socket_linux::leave_event_listener()
	{
		if(epoll_linux::instance()->remove_handle( (int)this->get_handle() ) )
		{
			this->release();
		}
	}
    bool socket_linux::want_read()
    {

        return true;
    }
    bool socket_linux::want_write()
    {
        return true;
    }
}