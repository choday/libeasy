#include "socket_win32.hpp"
#include <Mswsock.h>
namespace eio
{

	void socket_win32::read_callback(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params)
	{
		int error_code = this->get_socket_error_code();
		if(0==error_code)
		{	
			if(this->_status.test(status_flags_stream) )
			{
				if(0==get_nread_size())//disconnected
				{
					this->process_closed();
				}else
				{
					this->process_readable();
				}
			}else
			{
				this->_read_params.hold_buffer.resize(complete_bytes); 
				this->_read_cache.lock();
				this->_read_cache.data = this->_read_params.hold_buffer;
				this->_read_cache.address = this->_read_params.hold_address;
				this->_read_cache.has_data=true;
				this->_read_cache.unlock();
				this->process_readable();
			}
		}else
		{
			if(error_code==64)this->process_closed();
			else this->process_error(true);
		}
		
		this->_read_params.reset();
		this->release();
	}

	void socket_win32::native_want_read()
	{
		if( is_invalid_handle() )return;
		if( this->set_flags(socket_status_flags_want_read) & socket_status_flags_want_read )return;
		
		if(this->_status.value&status_flags_listened)
        {
            if(!this->post_accept())this->clear_flags(socket_status_flags_want_read);
            return;
        }
		
		int result = 0;
		DWORD error = 0;

		this->add_ref();
		
		if(this->_status.value&status_flags_stream)
		{
			result =::WSARecv( this->_handle,_read_params.init_buffer(0),1,&_read_params.completed_bytes,&_read_params.flags,_read_params.init(&socket_win32::read_callback,this),0 );
		}else
		{
			result =::WSARecvFrom( this->_handle,_read_params.init_buffer(this->_default_read_buffer_size),1,
									&_read_params.completed_bytes,&_read_params.flags,
									(sockaddr *)_read_params.hold_address.data(),&_read_params.hold_address_size,
									_read_params.init(&socket_win32::read_callback,this),0 );
			
		}
		error = ::WSAGetLastError();
		//有error==0的情况 ，发送0字节 的时候，会出现这个
		if(result && ERROR_IO_PENDING!=error)
		{
            this->clear_flags(socket_status_flags_want_read);
			this->process_error(true);
			this->release();
		}
	}

	int socket_win32::native_recv(void* data,int len)
	{
		if( is_invalid_handle() )return -1;

		int result = 0;
		DWORD error = 0;
	
		DWORD completed_bytes=0;
		DWORD flags = 0;
		WSABUF wsabuffer={0};

        wsabuffer.buf = (char*)data;
		wsabuffer.len = len;

		result =::WSARecv(this->_handle,&wsabuffer,1,&completed_bytes,&flags,0,0 );
		error = ::WSAGetLastError();

        if(0==result)
        {
            return completed_bytes;
        }else if( WSAEWOULDBLOCK==error )
        {
            return 0;
        }

        return -1;
	}

	bool socket_win32::fetch_read_cache(ebase::buffer& buffer,socket_address* address)
	{
		bool result = false;
		_read_cache.lock();
		if(_read_cache.has_data)
		{
			buffer = _read_cache.data;
			if( address )address->assign( _read_cache.address );
			_read_cache.has_data=false;
			result=true;
		}
		_read_cache.unlock();

		if(buffer.size()==0)result =false;

		return result;
	}

	int socket_win32::native_recvfrom(ebase::buffer& data,socket_address* from_address)
	{
        data.resize(0);

		if( is_invalid_handle() )return -1;

		if(fetch_read_cache(data,from_address))return data.size();
	
		int result = 0;
		DWORD error = 0;

		DWORD	completed_bytes=0;
		DWORD	flags = 0;
		WSABUF	wsabuffer={0};
        socket_address sssaddress;

		sockaddr *address = (sockaddr *)sssaddress.data();
		int		address_size = sssaddress.capacity();

		wsabuffer.buf = (char*)data.resize(this->_default_read_buffer_size,false);
		wsabuffer.len = data.capacity();

		result =::WSARecvFrom( this->_handle,&wsabuffer,1,&completed_bytes,&flags,address,&address_size,0,0 );
		error = ::WSAGetLastError();

        data.resize( completed_bytes );

		if(0==result)
		{
			if(from_address)from_address->assign( sssaddress );
			return completed_bytes;
		}else if(WSAEWOULDBLOCK==error)
        {
            return 0;
        }
        return -1;
	}

}