#pragma once
#include "../socket_native.hpp"

namespace eio
{

    class epoll_linux;
	class socket_linux:public socket_native
	{
        friend class epoll_linux;
	public:
		socket_linux(ebase::executor* event_executor=0);
		virtual bool		native_enter_listener() override;
		virtual void		native_leave_listener() override;
        virtual void        native_io_flags_changed() override;
        virtual void		native_want_read() override;
        virtual void		native_want_write() override;
	};
};