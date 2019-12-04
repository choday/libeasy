#include "socket_win32.hpp"
#include <Mswsock.h>

namespace eio
{

	void socket_win32::connect_callback(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params)
	{
		int error_code = this->get_socket_error_code();

		if(error_code)
		{
			this->process_error(true);//�Զ����status_flags_connectting��־ 
		}else
		{
			setsockopt( this->_handle,SOL_SOCKET,SO_UPDATE_CONNECT_CONTEXT,NULL,0 );
			
			this->process_writeable();//�Զ����status_flags_connectting��־
		}

		this->release();
	}

	int socket_win32::native_connect( const socket_address& address )
	{
		this->add_ref();

		BOOL result = lpfn_ConnectEx( this->_handle,(const sockaddr *)address.data(),address.size(),0,0,0,_write_params.init(&socket_win32::connect_callback,this) );
		DWORD error = ::WSAGetLastError();
		//���û��bind,�᷵��WSAEINVAL
		if(!result && error && ERROR_IO_PENDING!=error)
		{
			this->release();
			return false;
		}
		return true;
	}
}