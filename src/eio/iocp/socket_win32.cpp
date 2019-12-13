#include "socket_win32.hpp"
#include <mstcpip.h>

namespace eio
{


	LPFN_CONNECTEX	socket_win32::lpfn_ConnectEx=0;
	LPFN_ACCEPTEX	socket_win32::lpfn_AcceptEx = 0;
	LPFN_DISCONNECTEX	socket_win32::lpfn_DisConnectEx = 0;
	LPFN_GETACCEPTEXSOCKADDRS	socket_win32::lpfn_GetAcceptExSockaddrs = 0;

	bool socket_set_udp_connreset( SOCKET _socket_,bool on );

	socket_win32::socket_win32(ebase::executor* event_executor):socket_native(event_executor)
	{
		
		//ebase::executor_single_thread::set_parent( iocp_win32::instance() );

		_read_cache.has_data =false;
		_read_cache.accept_socket= INVALID_SOCKET;
	}

	socket_win32::~socket_win32()
	{

		if(INVALID_SOCKET!=_read_cache.accept_socket)closesocket(_read_cache.accept_socket);
		_read_cache.accept_socket=INVALID_SOCKET;

		
	}

	SOCKET socket_win32::native_create_socket(int af,int type,int protocol)
	{
		SOCKET s = ::WSASocket( af,type,protocol,0,0,WSA_FLAG_OVERLAPPED );

		if(INVALID_SOCKET!=s)
		{
			if(type!=SOCK_STREAM)socket_set_udp_connreset( this->_handle,false );
			init_winsock32_extension(s);
		}

		return s;
	}



    bool socket_win32::native_enter_listener()
	{
		return iocp_win32::instance()->assign_handle( (void*)this->_handle,this );
	}

	void socket_win32::native_leave_listener()
	{

	}

    void socket_win32::native_io_flags_changed()
    {

    }

    bool socket_win32::init_winsock32_extension(SOCKET handle)
	{
		DWORD			bytes=0;

		if( 0==lpfn_ConnectEx )
		{
			GUID			guid = WSAID_CONNECTEX;
			if( SOCKET_ERROR == WSAIoctl(handle,SIO_GET_EXTENSION_FUNCTION_POINTER,&guid,sizeof(guid),&lpfn_ConnectEx,sizeof(lpfn_ConnectEx),&bytes,0,0 ) )return false;
		}

		if( 0==lpfn_AcceptEx )
		{
			GUID			guid = WSAID_ACCEPTEX;
			if( SOCKET_ERROR == WSAIoctl(handle,SIO_GET_EXTENSION_FUNCTION_POINTER,&guid,sizeof(guid),&lpfn_AcceptEx,sizeof(lpfn_AcceptEx),&bytes,0,0 ) )return false;
		}

		if( 0==lpfn_DisConnectEx )
		{
			GUID			guid = WSAID_DISCONNECTEX;
			if( SOCKET_ERROR == WSAIoctl(handle,SIO_GET_EXTENSION_FUNCTION_POINTER,&guid,sizeof(guid),&lpfn_DisConnectEx,sizeof(lpfn_DisConnectEx),&bytes,0,0 ) )return false;
		}

		if( 0==lpfn_GetAcceptExSockaddrs )
		{
			GUID			guid = WSAID_GETACCEPTEXSOCKADDRS;
			if( SOCKET_ERROR == WSAIoctl(handle,SIO_GET_EXTENSION_FUNCTION_POINTER,&guid,sizeof(guid),&lpfn_GetAcceptExSockaddrs,sizeof(lpfn_GetAcceptExSockaddrs),&bytes,0,0 ) )return false;
		}

		return true;
	}


	LPWSABUF socket_win32::params::init_buffer(const ebase::buffer& b)
	{
		this->wsabuffer.buf = (char*)b.data();
		this->wsabuffer.len = b.size();

		this->hold_buffer=b;
		this->flags=0;
		this->hold_address_size=this->hold_address.capacity();

		return &this->wsabuffer;
	}

	LPWSABUF socket_win32::params::init_buffer(int capacity/*=0*/)
	{

		if(capacity)
		{
			this->wsabuffer.buf = (char*)this->hold_buffer.alloc(capacity);
			this->wsabuffer.len = this->hold_buffer.capacity();
		}else
		{
			this->wsabuffer.buf = 0;
			this->wsabuffer.len = 0;
		}

		this->flags=0;
		this->hold_address_size=this->hold_address.capacity();

		return &this->wsabuffer; 
	}

	void socket_win32::params::reset()
	{
		this->hold_buffer.resize(0);
		this->flags=0;
	}


bool socket_set_udp_connreset( SOCKET _socket_,bool on )
{
	BOOL   bNewBehavior   =   on;   
	DWORD   dwBytesReturned; 

#ifndef SIO_UDP_CONNRESET
#define IOC_VENDOR 0x18000000 
#define _WSAIOW(x,y) (IOC_IN|(x)|(y)) 
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
#endif
	return 0 == WSAIoctl( (SOCKET)_socket_,SIO_UDP_CONNRESET,&bNewBehavior,sizeof(bNewBehavior),NULL,0,&dwBytesReturned,NULL,NULL);
}

};