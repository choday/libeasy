#include "socket_posix.hpp"
#include "poll_posix.hpp"
#include "select_posix.hpp"

#ifdef _WIN32

#include <WinSock2.h>
#define poll WSAPoll

#include "../iocp/iocp_win32.hpp"

#else
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#define ioctlsocket	ioctl
#define closesocket close
#endif


namespace eio
{
	socket_posix::socket_posix(ebase::executor* event_executor/*=0*/,bool use_select):socket_native(event_executor),_socket_rw_manager(0)
	{
        ebase::ref_tree::entry::set_holder(this);
        ebase::ref_list::entry::set_holder(this);
		if(use_select)
		{
			_socket_rw_manager = select_posix::instance();
		}else
		{
			_socket_rw_manager = poll_posix::instance();
		}
		//ebase::executor_single_thread::set_parent( _socket_rw_manager );
	}

    socket_posix::~socket_posix()
    {
        
    }

    bool socket_posix::native_enter_listener()
	{
		return this->_socket_rw_manager->add_socket(this);
	}

	void socket_posix::native_leave_listener()
	{
		this->_socket_rw_manager->remove_socket(this);
	}

    void socket_posix::native_io_flags_changed()
    {
        _socket_rw_manager->need_dispath();
    }

	int socket_posix::compare_rbtree_entry(ebase::ref_tree::entry* left_value)
	{
		socket_posix* p = (socket_posix*)left_value;
		if( p->_handle<this->_handle )return -1;
		if( p->_handle>this->_handle )return 1;
		return 0;
	}

	int socket_posix::compare_rbtree_find_value(void* find_value)
	{
        SOCKET s= *(SOCKET*)&find_value;
		if( s<this->_handle )return -1;
		if( s>this->_handle )return 1;
		return 0;
	}

};