#include "eio.hpp"
#include "../ebase/thread_loop.hpp"
#include "posix/poll_posix.hpp"

#ifdef _WIN32
#include "iocp/iocp_win32.hpp"
#else
#include "epoll/epoll_linux.hpp"
#endif
namespace eio
{
	ebase::thread_loop	single_io_thread_loop;
	ebase::thread_loop	multi_io_thread_loop;

	ebase::executor*	global_name_resolver_executor=0;
	ebase::executor*	global_file_io_executor=0;
	ebase::executor*	global_crypto_executor=0;
	ebase::executor*	global_netio_executor=0;
	ebase::executor*	global_log_executor=0;
	
	ebase::executor* get_netio_executor()
	{
		if(0==global_netio_executor)
		{
#ifdef _WIN32
			global_crypto_executor = iocp_win32::instance();
#else
            global_crypto_executor = epoll_linux::instance();
#endif
		}

		return global_netio_executor;
	}

	ebase::executor* get_name_executor()
	{
		if(0==global_name_resolver_executor)
		{
#ifdef _WIN32
			iocp_win32::init_ws2_32();
#endif
			global_name_resolver_executor=get_fileio_executor();
		}

		return global_name_resolver_executor;
	}

	ebase::executor* get_fileio_executor()
	{

		if(0==global_file_io_executor)
		{
			global_file_io_executor=&multi_io_thread_loop;
			multi_io_thread_loop.start(0);
		}
		return global_file_io_executor;
	}

	ebase::executor* get_crypto_executor()
	{
		if(0==global_crypto_executor)
		{
#ifdef _WIN32
			global_crypto_executor = iocp_win32::instance();
#else

#endif
		}
		return global_crypto_executor;
	}

	ebase::executor* get_log_executor()
	{
		if(0==global_log_executor)
		{
			global_log_executor=&single_io_thread_loop;
			single_io_thread_loop.start(1);
		}

		return global_log_executor;
	}

    ebase::thread_loop* get_network_platform_loop()
    {
#ifdef _WIN32
		return iocp_win32::instance();
#else
        return epoll_linux::instance();
#endif
    }

    ebase::thread_loop* get_network_poll_loop()
    {
        return poll_posix::instance();
    }

};