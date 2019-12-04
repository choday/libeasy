#pragma once

#include "socket_rw_manager_posix.hpp"

namespace eio
{
	class socket_posix;
	class poll_posix:public socket_rw_manager_posix
	{
	public:
		poll_posix();
		~poll_posix();

		static poll_posix* instance();


	private:
		static poll_posix*	_current;

		void			rebuild_POLLFD();
		void			process_event(SOCKET socket_fd,int events);

		int				fdarray_count;
		int				fdarray_count_capacity;
		void*			fdarray_data;

		virtual void    run() override;

	};
};