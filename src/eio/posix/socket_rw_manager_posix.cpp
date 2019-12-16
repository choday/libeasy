#include "socket_rw_manager_posix.hpp"
#include "socket_posix.hpp"
#include <stdio.h>
namespace eio
{

	socket_rw_manager_posix::socket_rw_manager_posix()
	{
		socketpair_for_event[0] = -1;
		socketpair_for_event[1] = -1;
				
		socket_native::socket_udp_pair(socketpair_for_event);

        _fd_to_socket_map.set_entry_compare(this);
		_dispath_event_fd = socketpair_for_event[0];
	}

	socket_rw_manager_posix::~socket_rw_manager_posix()
    {
		socket_native::close_udp_pair(socketpair_for_event);
		socketpair_for_event[0] = -1;
		socketpair_for_event[1] = -1;
		_dispath_event_fd = -1;
				
		ebase::mutex_lock_scope(&this->lock_alllist);
		_manager_list.clear();

	}

	bool socket_rw_manager_posix::start()
	{
		return ebase::thread::start(1, true);
	}

	bool socket_rw_manager_posix::add_socket(socket_posix* ptr)
	{
		ebase::mutex_lock_scope(&this->lock_alllist);

		_fd_to_socket_map.insert_equal( &ptr->tree_entry );
		bool result = _manager_list.push_back( &ptr->list_entry );
        return result;
	}

	bool socket_rw_manager_posix::remove_socket(socket_posix* ptr)
	{
		ebase::mutex_lock_scope(&this->lock_alllist);

		_fd_to_socket_map.remove(&ptr->tree_entry);
		return _manager_list.remove( &ptr->list_entry );
	}


	socket_posix* socket_rw_manager_posix::find_socket_posix(SOCKET socket_fd)
	{
		ebase::shared_lock_scope(&this->lock_alllist);
        void* value = 0;
        *(SOCKET*)&value = socket_fd;

		ebase::ref_tree::entry* entry = _fd_to_socket_map.find(value);
        if(0==entry)
        {
            return 0;
        }

		return entry->get_holder<socket_posix>();
	}

	void socket_rw_manager_posix::need_dispath()
	{
		char data[1] = {0};
		data[0]='1';

		if(-1!=socketpair_for_event[1])::send( socketpair_for_event[1],data,1,0 );
	}

	void socket_rw_manager_posix::clear_socket_pair_event()
	{
		char temp[16];

        int result = ::recv(_dispath_event_fd, temp, 16, 0);
	
		while( ::recv( _dispath_event_fd,temp,16 ,0 ) >= 0){}

		int error = socket_native::get_last_error();

		return;
	}

    int socket_rw_manager_posix::compare_rbtree_entry(ebase::ref_tree::entry* left_value,ebase::ref_tree::entry* right_value)
    {
        socket_posix* left = left_value->get_holder<socket_posix>();
        socket_posix* right = right_value->get_holder<socket_posix>();

		if( left->get_handle()<right->get_handle() )return -1;
		if( left->get_handle()>right->get_handle() )return 1;
		return 0;
    }

    int socket_rw_manager_posix::compare_rbtree_find_value(ebase::ref_tree::entry* left_value, void* pfind_value)
    {
        SOCKET value = *(SOCKET*)&pfind_value;
        socket_posix* left = left_value->get_holder<socket_posix>();

		if( left->get_handle()<value )return -1;
		if( left->get_handle()>value )return 1;
		return 0;
    }

};