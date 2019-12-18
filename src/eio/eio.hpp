#pragma once

#include "../ebase/thread_loop.hpp"
/**@brief io名字空间，所有io操作类均在此类下面
*/
namespace eio
{
	ebase::executor*	get_netio_executor();//多线程
	ebase::executor*	get_name_executor();//单线程
	ebase::executor*	get_fileio_executor();//多线程，允许短时间io中断,不允许等待
	ebase::executor*	get_crypto_executor();//多线程，允许运算量较大的加密运算
	ebase::executor*	get_log_executor();//单线程

    ebase::thread_loop* get_network_platform_loop();
    ebase::thread_loop* get_network_poll_loop();
};