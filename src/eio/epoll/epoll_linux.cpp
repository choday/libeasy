#include "epoll_linux.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include "socket_linux.hpp"
namespace eio
{

	epoll_linux		global_epoll_linux;
	epoll_linux*	epoll_linux::_current=0;

	epoll_linux::epoll_linux():_epoll_fd(-1)
	{
		socketpair_for_event[0] = -1;
		socketpair_for_event[1] = -1;
				
		socket::socket_udp_pair(socketpair_for_event);

		_dispath_event_fd = socketpair_for_event[0];

	}

	epoll_linux::~epoll_linux()
	{

		socket::close_udp_pair(socketpair_for_event);
		socketpair_for_event[0] = -1;
		socketpair_for_event[1] = -1;
		_dispath_event_fd = -1;

		close(_epoll_fd);
		_epoll_fd=0;
	}
	void epoll_linux::clear_socket_pair_event()
	{
		char temp[16];

        int result = ::recv(_dispath_event_fd, temp, 16, 0);
	
		while( ::recv( _dispath_event_fd,temp,16 ,0 ) >= 0){}

		int error = socket::get_last_error();

		return;
	}
	void epoll_linux::start()
	{
		if(-1==_epoll_fd)
		{
			_epoll_fd = epoll_create(256);
            add_handle( _dispath_event_fd,&_dispath_event_fd );
			ebase::thread::start(1);
		}
	}

	epoll_linux* epoll_linux::instance()
	{
		if(0==epoll_linux::_current)
		{
			global_epoll_linux.start();
			epoll_linux::_current = &global_epoll_linux;
		}
		return epoll_linux::_current;
	}

	bool epoll_linux::remove_handle(int fd)
	{
		return 0 == epoll_ctl( _epoll_fd,EPOLL_CTL_DEL, fd, 0 );	
	}

	bool epoll_linux::add_handle(int fd,void* userdata)
	{
		struct epoll_event evt;  
        evt.events = EPOLLIN |EPOLLOUT | EPOLLET |  EPOLLERR | EPOLLHUP | EPOLLPRI;  
        evt.data.ptr = userdata; 

		//EPOLL_CTL_DEL
		int n = epoll_ctl( _epoll_fd,EPOLL_CTL_ADD, fd, &evt );	
		return n==0;
	}

	void epoll_linux::run()
	{
		int					event_list_size = 4096/sizeof(struct epoll_event);
		struct epoll_event	event_list[event_list_size];
		int					timeout_ms=1000;

		while(!this->is_aborted())
		{
			int ready_num = epoll_wait(_epoll_fd,event_list,event_list_size,timeout_ms );

			for( int i =0;i<ready_num;++i )
			{
				socket_linux* p=(socket_linux*)event_list[i].data.ptr;

		        if( &_dispath_event_fd == event_list[i].data.ptr && event_list[i].events)
		        {
				    clear_socket_pair_event();
			        continue;
		        }

                process_event(p,event_list[i].events);
			}

			this->dispath();
		}

        this->dispath();
	}

	void epoll_linux::need_dispath()
	{
		char data[1] = {0};
		data[0]='1';

		if(-1!=socketpair_for_event[1])::send( socketpair_for_event[1],data,1,0 );
	}

    void epoll_linux::process_event(socket_linux* p, uint32_t events)
    {

		if( EPOLLOUT & events )
		{
			p->notify_writeable();
		}

		if( EPOLLIN & events )
		{
			p->notify_readable();
		}

		if( EPOLLERR & events )
		{
			p->notify_error(true);
		}
		if( EPOLLHUP & events )
		{
			p->notify_closed();

		}
    }

}