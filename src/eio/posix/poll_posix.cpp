#include "poll_posix.hpp"
#include "socket_posix.hpp"
#include "../../ebase/allocator.hpp"
#include <stdio.h>

#ifdef _WIN32

#include <WinSock2.h>
#define poll WSAPoll
#define POLLFD  WSAPOLLFD
#else

#include <poll.h>
#define POLLFD struct pollfd
#endif

//https://blog.csdn.net/skypeng57/article/details/82743681
//POLLRDNORM is indicated in the returned revents member of WSAPOLLFD structure when a remote peer shuts down a send operation (a TCP FIN was received). A subsequent recv function request will return zero bytes.

namespace eio
{
	poll_posix		global_poll_posix;
	eio::poll_posix* poll_posix::_current = 0;

	poll_posix::poll_posix()
	{

		fdarray_count_capacity=0;
		fdarray_count = 0;
		fdarray_data = 0;

	}

	poll_posix::~poll_posix()
	{
        this->abort_all();
        this->join();

		if(fdarray_data)
		{
			ebase::allocator::free(fdarray_data);
			fdarray_data=0;
            fdarray_count=0;
            fdarray_count_capacity=0;
		}
	}

	void poll_posix::rebuild_POLLFD()
	{
        ebase::shared_lock_scope(&this->lock_alllist);

		int fds = _manager_list.size()+1;

		if(fdarray_count_capacity<fds)
		{
			fdarray_count_capacity = fds;

			if(fdarray_data)fdarray_data = ebase::allocator::realloc(fdarray_data,fdarray_count_capacity*sizeof(POLLFD));
			else fdarray_data = ebase::allocator::malloc(fdarray_count_capacity*sizeof(POLLFD));
		}

		POLLFD* fdarray = (POLLFD*)fdarray_data;
		fdarray_count = 0;

		fdarray[fdarray_count].fd = (SOCKET)_dispath_event_fd;
		fdarray[fdarray_count].events = POLLRDNORM;
		fdarray[fdarray_count].revents = 0;
		fdarray_count++;

		for( ebase::ref_list::entry* entry = _manager_list.begin();entry!=_manager_list.end();entry=entry->next() )
		{
			socket_posix* socket_handle = (socket_posix*)entry;

			int events = 0;//POLLHUP|POLLERR;

			if( socket_handle->test_flags(socket_posix::socket_status_flags_want_read) )
			{
				events |= POLLRDNORM;
			}else if( socket_handle->test_flags(socket_posix::socket_status_flags_want_write) )
			{
				events |= POLLWRNORM;
			}else
			{
				continue;
			}
			fdarray[fdarray_count].fd = (SOCKET)socket_handle->get_handle();

			fdarray[fdarray_count].events = events;
			fdarray[fdarray_count].revents = 0;
			fdarray_count++;
		}
	}

	void poll_posix::process_event(SOCKET socket_fd,int events)
	{

		if(_dispath_event_fd == (int)socket_fd)
		{
            
			if(events&POLLRDNORM)
				clear_socket_pair_event();
			return;
		}

		socket_posix* p = find_socket_posix(socket_fd);
		if(!p)return;

		if(POLLWRNORM & events )
		{
			p->clear_flags( socket_posix::socket_status_flags_want_write );
			p->process_writeable();
			
		}

		if(POLLRDNORM & events )
		{
			p->clear_flags( socket_posix::socket_status_flags_want_read );
			p->process_readable();
		}

		if(POLLERR & events )
		{
			p->process_error(true);
		}

		if(POLLHUP & events )
		{
			p->process_closed();
		}
				
	}

	void poll_posix::run()
	{
		int				timeout_ms = 10000;

		int error_code=0;

		while( !is_aborted() )
		{
	
			rebuild_POLLFD();

			POLLFD* fdarray = (POLLFD*)fdarray_data;
			int count = poll( fdarray,fdarray_count,timeout_ms );
            
			if( count <0 )
			{
				error_code = socket_native::get_last_error();

				break;
			}else if(count==0)
			{
			}else for(int i =0;i<fdarray_count;++i)
			{
				if(fdarray[i].revents)process_event( (SOCKET)fdarray[i].fd,fdarray[i].revents );
			}

			this->dispath();
		}

		if(fdarray_data)
		{
			ebase::allocator::free(fdarray_data);
			fdarray_data=0;
            fdarray_count=0;
            fdarray_count_capacity=0;
		}
	}


	poll_posix* poll_posix::instance()
	{
		if(0==poll_posix::_current)
		{
			global_poll_posix.start();
			poll_posix::_current= &global_poll_posix;
		}
		return poll_posix::_current;
	}
};