#pragma once
#include "../../ebase/thread_loop.hpp"
#include "../../ebase/allocator.hpp"
namespace eio
{

    class socket_linux;

	class epoll_linux:public ebase::thread_loop
	{
	public:
		epoll_linux();
		~epoll_linux();

		void                start();
		static epoll_linux* instance();
		bool		        add_handle(int fd,void* userdata);
		bool		        remove_handle(int fd);

	private:
		static epoll_linux* _current;
		int			        _epoll_fd;
		int				    _dispath_event_fd;
        int			        socketpair_for_event[2];//socketpair_for_event[0]用于接收消息

        void		        clear_socket_pair_event();
		virtual void        run() override;
		virtual void        need_dispath() override;

        void                process_event(socket_linux* p, uint32_t events);
	};
};