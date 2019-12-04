#pragma once
#include "../../ebase/thread_loop.hpp"
#include "../../ebase/allocator.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace eio
{
    class iocp_win32:public ebase::thread_loop
	{
	public:
		iocp_win32();
		~iocp_win32();

		static iocp_win32* instance();
		static void	init_ws2_32();

		bool		assign_handle(void* handle,void* io_key);

		struct params;

		typedef void (*lpfn_io_completed)(void* io_key,unsigned long complete_bytes,struct params* ol_params);

		struct params:public OVERLAPPED
		{
			LPOVERLAPPED	init(void* callback,void* class_ptr=0);

			//使用者自己保持class_ptr有效性，不能在回调前释放
			template<typename class_name>
			inline LPOVERLAPPED init(void (class_name::*function)(void* io_key,unsigned long complete_bytes,struct params* ol_params),void* class_ptr )
			{
				return this->init( *(void**)&function,class_ptr );
			}

			void				invoke(void* io_key,unsigned long complete_bytes,struct params* ol_params);

			void*				_callback;
			void*				_callback_class;
		};

		virtual void need_dispath() override;
        virtual void run() override;
	private:
		
		static iocp_win32* _current;
		HANDLE		 _handle;
        void        dispath_network( DWORD timeout_ms = 0 );
	};
};