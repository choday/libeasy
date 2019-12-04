#include "socket_win32.hpp"
#include <Mswsock.h>
namespace eio
{

	bool socket_win32::post_accept()
	{
		if( is_invalid_handle() )return false;

		SOCKET new_socket = INVALID_SOCKET;

		this->_read_cache.lock();
		{
			assert(!this->_read_cache.has_data);

			if(INVALID_SOCKET== this->_read_cache.accept_socket )this->_read_cache.accept_socket= this->native_create_socket( this->_family,SOCK_STREAM,IPPROTO_TCP );
			new_socket = this->_read_cache.accept_socket;
		}
		this->_read_cache.unlock();

		if(INVALID_SOCKET==new_socket)return false;


		int result = 0;
		DWORD error = 0;

		this->add_ref();
		
		int accept_data_capacity = 0;

		result = lpfn_AcceptEx( this->_handle,new_socket,
								_read_params.init_buffer(accept_data_capacity+((sizeof(sockaddr_in) + 16) * 2))->buf,accept_data_capacity,
								(sizeof(sockaddr_in) + 16),(sizeof(sockaddr_in) + 16),
								&_read_params.completed_bytes,_read_params.init(&socket_win32::accept_callback,this) );


		error = ::WSAGetLastError();
		//有error==0的情况 ，发送0字节 的时候，会出现这个
		if(!result && ERROR_IO_PENDING!=error)
		{
			this->release();
			return false;
		}
		
		return true;
	}

	
	void socket_win32::accept_callback(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params)
	{
		int error_code = this->get_socket_error_code();
		if(error_code==0)
		{
			this->_read_cache.lock();

			SOCKET new_socket = this->_read_cache.accept_socket;
			int result = setsockopt( new_socket,SOL_SOCKET,SO_UPDATE_ACCEPT_CONTEXT,(const char*)&this->_handle,sizeof(this->_handle) );
			if(-1==result)
			{
				::closesocket(new_socket);
				this->_read_cache.accept_socket=INVALID_SOCKET;
				this->_read_cache.has_data=false;
			}else
			{
				this->_read_cache.has_data =true;
			}
			
			this->_read_cache.unlock();

			this->process_readable();
		}else
		{
			this->process_error(true);
		}
		this->release();
	}

	
	socket_native_ptr socket_win32::accept()
	{
		if( is_invalid_handle() )return 0;

		SOCKET new_socket=INVALID_SOCKET;

		_read_cache.lock();
		if(_read_cache.has_data)
		{
			new_socket = _read_cache.accept_socket;
			_read_cache.accept_socket = INVALID_SOCKET;
			_read_cache.has_data=false;
		}
		_read_cache.unlock();

		if( INVALID_SOCKET != new_socket )
		{
			socket_native_ptr temp = socket_native::attach_accept_handle( new_socket );
			if( temp )return temp;
			
			this->process_error(false);
			return 0;
		}

		return socket_native::accept();
	}
}