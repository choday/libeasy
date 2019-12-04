#include "iocp_win32.hpp"
#include "../../ebase/thread.hpp"
#include <winsock2.h>
#include <stdio.h>
namespace eio
{
	iocp_win32* iocp_win32::_current = nullptr;

    iocp_win32            global_iocp_context;

	iocp_win32::iocp_win32():_handle(NULL)
	{
        init_ws2_32();
		_handle=CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL,0,0 );
	}

	iocp_win32::~iocp_win32()
	{
		CloseHandle(_handle);
        _handle = 0;
        this->abort_all();
        this->join();
	}

	bool iocp_win32::assign_handle(void* handle,void* io_key)
	{
		return NULL!=CreateIoCompletionPort( handle,this->_handle,(ULONG_PTR)io_key,0 );
	}

	void iocp_win32::init_ws2_32()
	{

		static bool ws32_inited=false;
		if(!ws32_inited)
		{
			WSADATA wsaData;
			int err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
			assert(err==0);

			ws32_inited=(err==0);
		}

	}

	iocp_win32* iocp_win32::instance()
	{
		if(iocp_win32::_current)return iocp_win32::_current;
		
        global_iocp_context.start(1);

        _current=&global_iocp_context;

		return iocp_win32::_current;
	}


    void iocp_win32::need_dispath()
	{
		PostQueuedCompletionStatus(_handle,0,0,0 );
	}


	LPOVERLAPPED iocp_win32::params::init(void* callback,void* class_ptr)
	{
		memset(this,0,sizeof(OVERLAPPED));
		this->_callback=callback;
		this->_callback_class = class_ptr;
		return this;
	}

	void iocp_win32::params::invoke(void* io_key,unsigned long complete_bytes,struct params* ol_params)
	{
		if(this->_callback_class)
		{
			typedef void (iocp_win32::params::*lpfn_io_class_completed)(void* io_key,unsigned long complete_bytes,struct params* ol_params);

			(((iocp_win32::params*)this->_callback_class)->*( *(lpfn_io_class_completed*)&this->_callback))( io_key,complete_bytes,ol_params );

		}else
		{

			lpfn_io_completed	fn = (lpfn_io_completed)this->_callback;

			fn( io_key,complete_bytes,ol_params );
		}
	}
    void iocp_win32::dispath_network(DWORD timeout_ms)
    {
		DWORD complete_bytes=0;
		ULONG_PTR key=0;
		params* iocp_data=0;

		BOOL result = GetQueuedCompletionStatus( _handle,&complete_bytes,&key,(LPOVERLAPPED *)&iocp_data,timeout_ms );
		DWORD error_code = GetLastError();
		if(result)error_code=0;

        //必须处理完
        if(0==iocp_data)
		{
			if(WAIT_TIMEOUT==error_code)
			{
			}
		}else
		{
			iocp_data->invoke( (void*)key,complete_bytes,iocp_data );
		}

    }
    
	void iocp_win32::run()
    {

        DWORD timeout_ms=1000;

		while(!is_aborted())
		{
            this->dispath_network(timeout_ms);

			this->dispath();
		}

        timeout_ms=0;
        this->dispath_network(timeout_ms);
        this->need_dispath();
	}
};