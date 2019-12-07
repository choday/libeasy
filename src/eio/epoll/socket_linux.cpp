#include "socket_linux.hpp"
#include "epoll_linux.hpp"
#include <stdio.h>
namespace eio
{

	socket_linux::socket_linux(ebase::executor* event_executor/*=0*/):socket_native(event_executor)
	{

	}

	bool socket_linux::native_enter_listener()
	{
		if(epoll_linux::instance()->add_handle( (int)this->get_handle(),this ) )
		{
			this->add_ref();
			return true;
		}
		return false;
	}

	void socket_linux::native_leave_listener()
	{
		if(epoll_linux::instance()->remove_handle( (int)this->get_handle() ) )
		{
			this->release();
		}
	}
    void socket_linux::native_io_flags_changed()
    {

    }
    void socket_linux::native_want_read()
    {

    }
    void socket_linux::native_want_write()
    {
    }
}