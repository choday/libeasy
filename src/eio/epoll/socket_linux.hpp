#pragma once
#include "../socket.hpp"

namespace eio
{

	class socket_linux:public socket
	{
	public:
		socket_linux(ebase::executor* event_executor=0);
		virtual bool		enter_event_listener() override;
		virtual void		leave_event_listener() override;
        virtual bool		want_read() override;
        virtual bool		want_write() override;
	};
};