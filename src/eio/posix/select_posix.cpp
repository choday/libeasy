#include "select_posix.hpp"
#include "socket_posix.hpp"

#ifdef _WIN32

#include <WinSock2.h>


#else


#endif


namespace eio
{
	select_posix		global_select_posix;
	eio::select_posix* select_posix::_current = 0;

	select_posix::select_posix()
	{

	}

	select_posix::~select_posix()
	{

	}


	void select_posix::run()
	{
		int				timeout_ms = 1000;

		while( 1 )
		{
		
			break;

			this->dispath();
		}

	}


	select_posix* select_posix::instance()
	{
		if(0==select_posix::_current)
		{
			global_select_posix.start();
			select_posix::_current= &global_select_posix;
		}
		return select_posix::_current;
	}
};