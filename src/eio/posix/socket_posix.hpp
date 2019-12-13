#pragma once
#include "../socket_native.hpp"
#include "../../ebase/executor.hpp"
#include "../../ebase/ref_list.hpp"
#include "../../ebase/ref_tree.hpp"

namespace eio
{

	class socket_rw_manager_posix;

	class socket_posix:public socket_native
	{
	public:
		socket_posix(ebase::executor* event_executor=0,bool use_select=true);
        ~socket_posix();

		virtual		bool		native_enter_listener() override;
		virtual		void		native_leave_listener() override;

		virtual void            native_io_flags_changed() override;

        ebase::ref_list::entry  list_entry;
        ebase::ref_tree::entry  tree_entry;
		friend class poll_posix;
	private:
		socket_rw_manager_posix*	_socket_rw_manager;
	};
};