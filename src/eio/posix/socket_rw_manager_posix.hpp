#pragma once
#include "../../ebase/ref_list.hpp"
#include "../../ebase/ref_tree.hpp"
#include "../../ebase/thread_loop.hpp"
#ifdef _WIN32
#include <WinSock2.h>
#else
#define SOCKET int
#endif

namespace eio
{

	//ʵ������Ҫʵ�����º���virtual void ebase::thread::run();


	class socket_posix;
	class socket_rw_manager_posix:public ebase::thread_loop
	{
	public:
		socket_rw_manager_posix();
		~socket_rw_manager_posix();


		bool		    start();

		bool		    add_socket(socket_posix* ptr);
		bool		    remove_socket(socket_posix* ptr);
		
		socket_posix*	find_socket_posix(SOCKET socket_fd);

		virtual void    need_dispath() override;//ebase::executor
	protected:
		ebase::ref_list	_manager_list;
		ebase::ref_tree	_fd_to_socket_map;
		ebase::mutex_rwlock	lock_alllist;

		int				_dispath_event_fd;//ʵ������Ҫ�������socket�Ķ��¼�,���ҵ���clear_socket_pair_event���socket����

		void		    clear_socket_pair_event();

	private:
		int			    socketpair_for_event[2];//socketpair_for_event[0]���ڽ�����Ϣ
	};
};