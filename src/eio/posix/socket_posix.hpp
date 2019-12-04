#pragma once
#include "../socket_native.hpp"
#include "../../ebase/executor.hpp"
#include "../../ebase/ref_list.hpp"
#include "../../ebase/ref_tree.hpp"

namespace eio
{

	class socket_rw_manager_posix;

	class socket_posix:public socket_native,public ebase::ref_list::entry,public ebase::ref_tree::entry
	{
	public:
		socket_posix(ebase::executor* event_executor=0,bool use_select=true);
        ~socket_posix();

		virtual		bool		native_enter_listener() override;
		virtual		void		native_leave_listener() override;

		virtual void            native_io_flags_changed() override;

		friend class poll_posix;
	private:
		socket_rw_manager_posix*	_socket_rw_manager;

	private://for ebase::ref_tree::entry
		virtual int		compare_rbtree_entry( ebase::ref_tree::entry* left_value );
		virtual int		compare_rbtree_find_value( void* pfind_value );
	};
};