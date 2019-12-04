#pragma once

#include "socket_rw_manager_posix.hpp"

namespace eio
{
	class select_posix:public socket_rw_manager_posix
	{
	public:
		select_posix();
		~select_posix();

		static select_posix* instance();

	private:
		static select_posix*	_current;
		virtual void run();
	};
};