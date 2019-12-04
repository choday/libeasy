#include "socket_win32.hpp"
#include <Mswsock.h>
namespace eio
{

	void socket_win32::write_callback(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params)
	{

		int error_code = this->get_socket_error_code();

		this->_write_params.reset();

		if(0==error_code)
		{
			this->process_writeable();
		}else
		{
			this->process_error(true);
		}
		
		this->release();
	}


	void socket_win32::native_want_write()
	{

	}

	bool socket_win32::send(const ebase::buffer& data)
	{
		if( is_invalid_handle() )return false;
		if(data.size()==0)return true;

        if(!this->test_flags(status_flags_writeable))return false;
        
	    int result = 0;
	    DWORD error = 0;

	    DWORD completed_bytes=0;
	    WSABUF wsabuffer={0};
	    wsabuffer.buf = (char*)data.data();
	    wsabuffer.len = data.size();

	    result =::WSASend( this->_handle,&wsabuffer,1,&completed_bytes,0,0,0 );
	    error = ::WSAGetLastError();
	    if(0==result  )
        {
            assert(completed_bytes==data.size());
            return true;
        }

        this->clear_flags(status_flags_writeable);
        if(SOCKET_ERROR == result && WSAEWOULDBLOCK!=error)return false;

        this->set_flags(socket_status_flags_want_write);
		this->add_ref();
		result =::WSASend( this->_handle,_write_params.init_buffer(data),1,&_write_params.completed_bytes,0,_write_params.init(&socket_win32::write_callback,this),0 );
		error = ::WSAGetLastError();
		if(SOCKET_ERROR == result && ERROR_IO_PENDING!=error)
		{
            this->clear_flags(socket_status_flags_want_write);
			this->release();
			return false;
		}

        return true;
	}

	bool socket_win32::sendto(const ebase::buffer& buffer,const socket_address& address)
	{
		if( is_invalid_handle() && !this->init( address.family(),SOCK_DGRAM,IPPROTO_UDP) )return false;

        //居然可以不绑定,也能sendto
		
		if(!_status.test(status_flags_bind))
		{
			socket_address addr_bind;
			addr_bind.set_family(address.family());
			if(!this->bind(addr_bind))return false;
		}

        if(buffer.size()==0)return true;

        if(!this->test_flags(status_flags_writeable))return false;
        
		int result = 0;
		DWORD error = 0;

        DWORD completed_bytes=0;
		WSABUF wsabuffer={0};
		wsabuffer.buf = (char*)buffer.data();
		wsabuffer.len = buffer.size();

		result=::WSASendTo( this->_handle,&wsabuffer,1,&completed_bytes,0,(const sockaddr *)address.data(),address.size(),0,0 );
		error = ::WSAGetLastError();
		if(0==result)return true;

        this->clear_flags(status_flags_writeable);
        if(SOCKET_ERROR == result && WSAEWOULDBLOCK!=error)return false;

		//永远不会触发WSAEWOULDBLOCK
        this->set_flags(socket_status_flags_want_write);
		this->add_ref();
		result=::WSASendTo( this->_handle,_write_params.init_buffer(buffer),1,&_write_params.completed_bytes,0,(const sockaddr *)address.data(),address.size(),_write_params.init(&socket_win32::write_callback,this),0 );
		error = ::WSAGetLastError();

		if( SOCKET_ERROR == result && ERROR_IO_PENDING!=error)
        {
            this->clear_flags(socket_status_flags_want_write);
            this->release();
            return false;
        }

		return true;
	}
}