#pragma once

#include "../ebase/thread_loop.hpp"
/**@brief io���ֿռ䣬����io��������ڴ�������
*/
namespace eio
{
	ebase::executor*	get_netio_executor();//���߳�
	ebase::executor*	get_name_executor();//���߳�
	ebase::executor*	get_fileio_executor();//���̣߳������ʱ��io�ж�,������ȴ�
	ebase::executor*	get_crypto_executor();//���̣߳������������ϴ�ļ�������
	ebase::executor*	get_log_executor();//���߳�

    ebase::thread_loop* get_network_platform_loop();
    ebase::thread_loop* get_network_poll_loop();
};