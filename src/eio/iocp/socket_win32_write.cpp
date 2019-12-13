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
	
    int socket_win32::native_send( const void* data,int len )
    {
		if( is_invalid_handle() )return -1;

	    int result = 0;
	    DWORD error_code = 0;

	    DWORD completed_bytes=0;
	    WSABUF wsabuffer={0};
	    wsabuffer.buf = (char*)data;
	    wsabuffer.len = len;

	    result =::WSASend( this->_handle,&wsabuffer,1,&completed_bytes,0,0,0 );
	    error_code = ::WSAGetLastError();
	    if(0==result  )
        {
            assert(completed_bytes==len);
            return completed_bytes;
        }

        if(WSAEWOULDBLOCK ==error_code)return 0;

        return -1;
    }

    int socket_win32::post_write(const ebase::buffer& data)
    {
        if( this->set_flags(socket_status_flags_want_write) & socket_status_flags_want_write )return 0;

		this->add_ref();
		int result =::WSASend( this->_handle,_write_params.init_buffer(data),1,&_write_params.completed_bytes,0,_write_params.init(&socket_win32::write_callback,this),0 );
		int error = ::WSAGetLastError();
		if(SOCKET_ERROR == result && ERROR_IO_PENDING!=error)
		{
            this->clear_flags(socket_status_flags_want_write);
			this->release();
			return -1;
		}

        return data.size();
    }

    int socket_win32::write( const void* data,int len ) 
    {
		if(len==0)return 0;

        if(!this->test_flags(status_flags_writeable))return 0;
        
        int result = native_send(data,len);
        if(result>0)return result;

        this->clear_flags(status_flags_writeable);

        if( -1 == result )
        {
            this->process_error(true);

        }else if(0==result)
        {
            ebase::buffer bb;
            bb.assign(data,len);
            result = this->post_write(bb);
        }

        return result;
    }

	int socket_win32::write_buffer(const ebase::buffer& data)
	{
		if(data.size()==0)return 0;

        if(!this->test_flags(status_flags_writeable))return 0;
        
        int result = native_send(data.data(),data.size() );
        if(result>0)return result;

        this->clear_flags(status_flags_writeable);

        if( -1 == result )
        {
            this->process_error(true);

        }else if(0==result)
        {
            result = this->post_write(data);
        }

        return result;
	}

	int socket_win32::sendto(const ebase::buffer& buffer,const socket_address& address)
	{
		if( is_invalid_handle() && !this->init( address.family(),SOCK_DGRAM,IPPROTO_UDP) )return -1;

        //居然可以不绑定,也能sendto
		
		if(!_status.test(status_flags_bind))
		{
			socket_address addr_bind;
			addr_bind.set_family(address.family());
			if(!this->bind(addr_bind))return -1;
		}

        if(buffer.size()==0)return 0;

        if(!this->test_flags(status_flags_writeable))return 0;
        
		int result = 0;
		DWORD error = 0;

        DWORD completed_bytes=0;
		WSABUF wsabuffer={0};
		wsabuffer.buf = (char*)buffer.data();
		wsabuffer.len = buffer.size();

		result=::WSASendTo( this->_handle,&wsabuffer,1,&completed_bytes,0,(const sockaddr *)address.data(),address.size(),0,0 );
		error = ::WSAGetLastError();
		if(0==result)return completed_bytes;

        this->clear_flags(status_flags_writeable);
        if(SOCKET_ERROR == result && WSAEWOULDBLOCK!=error)return -1;

		//永远不会触发WSAEWOULDBLOCK
        this->set_flags(socket_status_flags_want_write);
		this->add_ref();
		result=::WSASendTo( this->_handle,_write_params.init_buffer(buffer),1,&_write_params.completed_bytes,0,(const sockaddr *)address.data(),address.size(),_write_params.init(&socket_win32::write_callback,this),0 );
		error = ::WSAGetLastError();

		if( SOCKET_ERROR == result && ERROR_IO_PENDING!=error)
        {
            this->clear_flags(socket_status_flags_want_write);
            this->release();
            return -1;
        }

		return buffer.size();
	}
}