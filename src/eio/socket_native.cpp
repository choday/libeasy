#include "socket_native.hpp"
#include "name_resolver.hpp"
#include "eio.hpp"

#include "posix/socket_posix.hpp"
//在Linux平台下，可以通过在connect之前设置SO_SNDTIMO来达到控制连接超时的目的。

#ifdef _WIN32
#include <WinSock2.h>
#include <Mstcpip.h>
#include "iocp/socket_win32.hpp"
#define SOCKET_WOULDBLOCK WSAEWOULDBLOCK
#define socklen_t int
#else
#include <netdb.h>   
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#include "epoll/socket_linux.hpp"
#define ioctlsocket	ioctl
#define closesocket close
#define SOCKET_WOULDBLOCK EINPROGRESS
#endif


namespace eio
{
    int global_default_socket_api_type=socket_native::socket_api_platform;

	int socket_native::get_last_error()
	{
#ifdef _WIN32
		return ::WSAGetLastError();
#else
		return errno;
#endif
	}

	long socket_native::set_flags(long mask)
	{
		return this->_status.set(mask);
	}

	bool socket_native::test_flags(long mask)
	{
		return this->_status.test(mask);
	}

	long socket_native::clear_flags(long mask)
	{
		return this->_status.clear(mask);
	}

    
    void socket_native::set_default_api_type(int socket_api_type/*=socket_api_platform*/)
    {
        global_default_socket_api_type = socket_api_type;
    }

    eio::socket_native_ptr socket_native::create_instance(ebase::executor* event_executor,int socket_api_type)
	{
        if(socket_api_type==socket_api_default)socket_api_type = global_default_socket_api_type;

        if(socket_api_platform==socket_api_type)
        {
#ifdef _WIN32
		return new socket_win32(event_executor);
#else
		return new socket_linux(event_executor);
#endif
        }else //if(socket_api_poll==socket_api_type)
        {
            return new socket_posix(event_executor,false);
        }
	}


    socket_native::socket_native(ebase::executor* event_executor/*=0*/):_event_executor(0),_default_read_buffer_size(socket_default_read_buffer_size)
															,_handle(INVALID_SOCKET),_family(0)

	{
        socket_io::set_event_executor(event_executor);
	}

	socket_native::~socket_native()
	{
		if(status_flags_socket_valid&this->clear_flags(status_flags_socket_valid) )
		{
			::closesocket(_handle);
			_handle=INVALID_SOCKET;
		}
	}

	SOCKET	socket_native::get_handle() const
	{
		return _handle;
	}

	bool socket_native::is_invalid_handle() const
	{
		return  INVALID_SOCKET == _handle;
	}

	void socket_native::close()
	{
		if(status_flags_socket_valid&this->clear_flags(status_flags_socket_valid) )
		{
			this->native_leave_listener();

			::closesocket(_handle);

			_handle=INVALID_SOCKET;
            process_closed();
		}
	}

    void socket_native::close_after_send()
    {
        if(status_flags_writeable&this->set_flags(status_flags_close_after_send))
        {
            this->close();
        }
    }

    bool socket_native::init(int af,int type,int protocol)
	{
		if(!is_invalid_handle())close();

		this->_error.clear();

		_family= af;
		_status.reset((type==SOCK_STREAM?status_flags_stream:0));

		_handle = this->native_create_socket( af,type,protocol );
		if(is_invalid_handle())return false;
	
		this->set_nonblock_mode();

		_status.set(status_flags_socket_valid);

		this->native_enter_listener();
		
		return true;
	}

	bool socket_native::open(const ebase::string& host,const ebase::string& port_or_service)
	{
		return name_resolver::resolver( host,port_or_service,io_callback::bind( &socket_native::resolver_and_connect,this),this->_event_executor );
	}

    bool socket_native::open(const socket_address& address)
    {
        if(status_flags_connectting & this->_status.set(status_flags_connectting) )return false;

        do 
        {
		    if(is_invalid_handle() || (_family && _family!=address.family() ) )
		    {
			    if(!this->init( address.family(),SOCK_STREAM,IPPROTO_TCP) )break;
		    }

		    if(!_status.test(status_flags_bind))
		    {
			    socket_address addr_bind;
			    addr_bind.set_family(address.family());
			    if(!this->bind(addr_bind))break;
		    }

            this->_status.set(status_flags_connectting);

            int result = native_connect(address);
            if(-1==result)break;

            this->native_want_write();

            return true;
        } while (0);
	
        this->_status.clear(status_flags_connectting);

        return false;
    }

	bool socket_native::send(const ebase::buffer& data)
	{
        if(this->test_flags(status_flags_writeable))
        {
            int result = native_send(data);
            if(1==result)return true;

            this->clear_flags(status_flags_writeable);

            if( -1 == result )
            {
                this->process_error(true);

            }else if(0==result)
            {
                this->native_want_write();
            }
        }
        return false;
	}

    bool socket_native::sendto(const ebase::buffer& data,const socket_address& address)
	{
        if(this->test_flags(status_flags_writeable))
        {
            int result = native_sendto(data,address);
            if(1==result)return true;

            this->clear_flags(status_flags_writeable);

            if( -1 == result )
            {
                this->process_error(true);

            }else if(0==result)
            {
                this->native_want_write();
            }
        }
        return false;
	}

	bool socket_native::recv(ebase::buffer& data)
	{
        if(this->test_flags(status_flags_readable))
        {
            if(0==data.capacity())data.resize( 1024-ebase::buffer::header_size ,false );
            int result = native_recv(data);
            if(1==result)return true;

            data.resize(0,false);

            this->clear_flags(status_flags_readable);

            if( -1 == result )
            {
                this->process_error(true);

            }else if(0==result)
            {
                this->native_want_read();
            }
        }
        return false;
	}

    bool socket_native::recvfrom(ebase::buffer& data,socket_address* from_address/*=0*/)
	{
        if(this->test_flags(status_flags_readable))
        {
            int result = native_recvfrom(data,from_address);
            if(1==result)return true;

            this->clear_flags(status_flags_readable);

            if( -1 == result )
            {
                this->process_error(true);

            }else if(0==result)
            {
                this->native_want_read();
            }
        }
        return false;
	}

    bool socket_native::bind(const socket_address& address)
	{
		if(is_invalid_handle() || (_family && _family!=address.family() ))
		{
			if(!this->init( address.family(),SOCK_DGRAM,IPPROTO_UDP) )return false;
		}

		if( SOCKET_ERROR==::bind( this->_handle,(struct sockaddr*)address.data(),address.size() ) )return false;

		_status.set(status_flags_bind);
		if(!_status.test(status_flags_stream))
		{
			this->native_want_read();
			_status.set(status_flags_writeable);
		}
		return true;
	}

	bool socket_native::listen(const socket_address& address,int backlog/*=0*/)
	{
		if( is_invalid_handle() )
		{
			if(!this->init( address.family(),SOCK_STREAM,IPPROTO_TCP) )return false;
			if(!this->bind(address))return false;
		}
        if (0 == backlog)backlog = 20;//mobile 上不允许为0,否则虽然能listen成功，但是不会有accpt信号，connect也会认为是超时
		if(SOCKET_ERROR == ::listen( this->_handle,backlog ) )return false;

		this->_status.set(status_flags_listened);
		this->native_want_read();
		return true;
	}

	socket_native_ptr socket_native::accept()
	{
		if( is_invalid_handle() )return 0;

		SOCKET s= ::accept( this->_handle,0,0 );
		int error_code = get_last_error();

		if( INVALID_SOCKET == s )
		{
			if(SOCKET_WOULDBLOCK ==error_code)this->native_want_read();
			else this->process_error(true);
			return 0;
		}else
		{
			socket_native_ptr temp = socket_native::attach_accept_handle( s );
			if( temp )return temp;
			::closesocket(s);

			this->process_error(false);
		}
		return 0;
	}

	socket_native_ptr socket_native::attach_accept_handle(SOCKET s)
	{
		if(INVALID_SOCKET==s)return 0;

		socket_native_ptr temp = socket_native::create_instance( _event_executor );

		temp->_handle = s;
        /*
		if(temp->get_connect_time()<0)
		{
			temp->_handle = INVALID_SOCKET;
			return 0;
		}*/

		if(!temp->set_nonblock_mode())return 0;

		temp->_status.reset(status_flags_stream|status_flags_connected|status_flags_writeable|status_flags_socket_valid);

		temp->native_enter_listener();
		temp->native_want_read();

		return temp;
	}

	int socket_native::native_connect(const socket_address& address)
	{
        int result = ::connect( this->_handle,(const struct sockaddr*)address.data(),address.size() );
        if(result>=0)return 1;

		int error_code = get_last_error();
        if(SOCKET_WOULDBLOCK ==error_code)return 0;

        return -1;
	}

    int socket_native::native_send(const ebase::buffer& data)
    {
		int result = ::send( this->_handle,(const char*)data.data(),data.size(),0 );
        if(result>=0)return 1;

		int error_code = get_last_error();
        if(SOCKET_WOULDBLOCK ==error_code)return 0;

        return -1;
    }
    
    int socket_native::native_recv(ebase::buffer& data)
    {
		void* p = data.resize(0);

		int result = ::recv( this->_handle,(char*)p,data.capacity(),0 );
		if(result>=0)
		{
			data.resize(result,true);
			return 1;
		}
		data.resize(0,false);

		int error_code = get_last_error();

		if(SOCKET_WOULDBLOCK ==error_code)return 0;

		return -1;
    }

    int socket_native::native_sendto(const ebase::buffer& data,const socket_address& to_address)
    {
		int result = ::sendto( this->_handle,(const char*)data.data(),data.size(),0,(const struct sockaddr*)to_address.data(),to_address.size() );
		if(result>=0)return 1;

		int error_code = get_last_error();

		if(SOCKET_WOULDBLOCK ==error_code)return 0;

		return -1;
    }

    int socket_native::native_recvfrom(ebase::buffer& data,socket_address* from_address/*=0 */)
    {
		socket_address address;
        socklen_t fromlen=address.capacity();

		void* p = data.alloc(_default_read_buffer_size);
		int result = ::recvfrom( this->_handle,(char*)p,data.capacity(),0,(struct sockaddr*)address.data(),&fromlen );
		if(result>=0)
		{
			if(from_address)from_address->assign(address);

			data.resize(result,true);
			return 1;
		}
		data.resize(0,false);

		int error_code = get_last_error();

		if(SOCKET_WOULDBLOCK ==error_code)return 0;

		return -1;
    }

    int socket_native::get_error_code() const
    {
        return this->_error.code;
    }

    const char* socket_native::get_error_message() const
    {
        return this->_error.message.c_str();
    }

    void socket_native::resolver_and_connect(io_request* request)
	{
		resolver_request_ptr = (name_resolver::request*)request;

		if( !resolver_request_ptr->is_success() )
		{
			this->_error=resolver_request_ptr->error;
			resolver_request_ptr = 0;
			this->on_error.fire();
			return ;
		}

		process_resolver_and_connect();
	}

	
	bool socket_native::process_resolver_and_connect()
	{
		socket_address address;

		if(!resolver_request_ptr)return false;
		if(!resolver_request_ptr->current())return false;

		address.assign(resolver_request_ptr->current(),resolver_request_ptr->current_size() );
        resolver_request_ptr->move_next();

		bool result = this->open( address );
		if(!result)
		{
			resolver_request_ptr = 0;
			this->on_error.fire();
		}

		return result;
	}

	void socket_native::set_default_read_buffer_size(int capcity )
	{
		this->_default_read_buffer_size = capcity;
	}

	void socket_native::process_closed()
	{
		if(this->_status.test(status_flags_socket_valid))
		{
			this->_status.clear(status_flags_socket_valid|status_flags_connected|status_flags_writeable);
			socket_io::notify_closed(this);
		}
	}

	void socket_native::process_error(bool self_socket_error)
	{
		if(!self_socket_error)
		{
			this->_error.set_system_error(get_last_error());
            socket_io::notify_error(this);
			return ;
		}
		
		int error_code = this->get_socket_error_code();
		if(-1==error_code)return;//已关闭
		if( this->_status.test(status_flags_connectting) )//正在连接
		{
			this->_status.clear(status_flags_connectting);

			if(this->process_resolver_and_connect())return;
		}

		this->_error.set_system_error(error_code);
		socket_io::notify_error(this);
		this->close();
	}

	void socket_native::process_readable()
	{
        this->_status.clear(socket_status_flags_want_read);
		this->_status.set(status_flags_readable);
        socket_io::notify_readable(this);
	}

	void socket_native::process_writeable()
	{
        this->_status.clear(socket_status_flags_want_write);
        long ff = this->_status.set(status_flags_writeable);

		if(  ff & status_flags_connectting )
		{
			this->_status.clear(status_flags_connectting);

			this->resolver_request_ptr=0;
			this->_status.set(status_flags_connected);
			this->native_want_read();

			socket_io::notify_opened(this);
		}else if(ff & status_flags_close_after_send)
        {
            this->close();
        }else
		{
			socket_io::notify_writeable(this);
		}
	}

    void socket_native::native_want_read()
    {
        if( this->set_flags(socket_status_flags_want_read) & socket_status_flags_want_read )return;

        this->native_io_flags_changed();
    }

    void socket_native::native_want_write()
    {
        if( this->set_flags(socket_status_flags_want_write) & socket_status_flags_want_write )return;
        this->native_io_flags_changed();
    }

    SOCKET socket_native::native_create_socket(int af,int type,int protocol)
	{
		return ::socket( af,type,protocol );
	}

	bool socket_native::is_writeable()
	{
		return this->_status.test(status_flags_writeable);
	}

	bool socket_native::is_readable()
	{
		return this->_status.test(status_flags_readable);
	}

	bool socket_native::is_opened()
	{
		return (this->_status.value & status_flags_connected)!=0;
	}

	bool socket_native::is_stream()
	{
		return (this->_status.value & status_flags_stream)!=0;
	}

	socket_address socket_native::get_local_address() const
	{
		socket_address result;
        socklen_t len = result.capacity();

		::getsockname( this->_handle,(sockaddr *)result.data(),&len );
		return result;
	}

	socket_address socket_native::get_remote_address() const
	{
		socket_address result;
        socklen_t len = result.capacity();

		::getpeername( this->_handle,(sockaddr *)result.data(),&len );

		return result;
	}

	bool socket_native::set_keepalive(bool enable,int idle_seconds /*= 60*/,int interval_seconds /*= 5*/,int count)
	{

		SOCKET fd = this->_handle;
#ifdef _WIN32
		struct tcp_keepalive value = {0}, result = {0} ;
		DWORD	dwBytes = 0;
		value.onoff = enable ;
		value.keepalivetime = idle_seconds*1000 ; // Keep Alive in 5.5 sec.
		value.keepaliveinterval = interval_seconds*1000 ; // Resend if No-Reply 
		return 0==WSAIoctl( fd, SIO_KEEPALIVE_VALS, &value, sizeof(value), &result, sizeof(result), &dwBytes, NULL, NULL);

#else
		int value = (int)enable;

		if(-1==::setsockopt( fd,SOL_SOCKET,SO_KEEPALIVE,(const char*)&value,sizeof(value) ))return false;
		if(!enable)return true;

		setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void*)&idle_seconds, sizeof(idle_seconds));

		setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&interval_seconds, sizeof(interval_seconds));

		setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&count, sizeof(count));
#endif
		return true;

	}

	bool socket_native::set_send_buffer_size(int maxbuffersize)
	{
		SOCKET fd = this->_handle;
		return 0== setsockopt( fd,SOL_SOCKET,SO_SNDBUF,(char*)&maxbuffersize,sizeof(maxbuffersize));
	}

	bool socket_native::set_recv_buffer_size(int maxbuffersize)
	{
		SOCKET fd = this->_handle;
		return 0== setsockopt( fd,SOL_SOCKET,SO_RCVBUF,(char*)&maxbuffersize,sizeof(maxbuffersize));
	}

	bool socket_native::set_nonblock_mode()
	{
		int nonblock_mode_on = 1;

		int result = ioctlsocket(this->_handle, FIONBIO, (u_long *)&nonblock_mode_on);

		return result==0;
	}

	int socket_native::get_socket_error_code()
	{
		if(is_invalid_handle())return -1;

		int value = 0;	
        socklen_t len = sizeof(value);

		if(0==::getsockopt( _handle,SOL_SOCKET,SO_ERROR,(char*)&value,&len ))return value;
		return -1;
	}

	int socket_native::get_socket_type()
	{
		if(is_invalid_handle())return -1;

		int value = 0;	
        socklen_t len = sizeof(value);

		if(0 == getsockopt( this->_handle,SOL_SOCKET,SO_TYPE,(char *)&value,&len ) )return value;
		return -1;
	}

	int socket_native::get_nread_size()
	{
		u_long size = 0;
		int result = 0;
		result = ::ioctlsocket(_handle,FIONREAD,&size );
		if(result)return -1;
		return size;
	}

	bool _socket_set_nonblock_mode(SOCKET s)
	{
		int nonblock_mode_on = 1;

		int result = ioctlsocket(s, FIONBIO, (u_long *)&nonblock_mode_on);

		return result==0;
	}

	int socket_native::socket_udp_pair(int sv[2])
	{
		SOCKET out[2];
#ifdef _WIN32

		iocp_win32::init_ws2_32();

		out[0] = ::socket( AF_INET,SOCK_DGRAM,0 );
		if(INVALID_SOCKET==out[0])return -1;

		out[1] = ::socket( AF_INET,SOCK_DGRAM,0 );
		if(INVALID_SOCKET==out[1])
		{
			closesocket(out[0]);
			return -1;
		}

		do 
		{

			sockaddr_in addr={0};
			int addrlen = sizeof(addr);

			addr.sin_family=AF_INET;
			addr.sin_port = 0;
			//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
            ::inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr );

			if( -1==::bind(out[0],(sockaddr*)&addr,addrlen))break;
			if( -1==::getsockname(out[0],(sockaddr*)&addr,&addrlen))break;

			sockaddr_in addr2={0};
			int addrlen2 = sizeof(addr2);

			addr2.sin_family=AF_INET;
			addr2.sin_port = 0;
			//addr2.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
            ::inet_pton(AF_INET,"127.0.0.1",&addr2.sin_addr );

			if( -1==::bind(out[1],(sockaddr*)&addr2,addrlen2))break;
			if( -1==::getsockname(out[1],(sockaddr*)&addr2,&addrlen2))break;

			if( -1==::connect( out[0],(const sockaddr*)&addr2,addrlen2) )break;
			if( -1==::connect( out[1],(const sockaddr*)&addr,addrlen) )break;

			if(!_socket_set_nonblock_mode(out[0]))break;
			if(!_socket_set_nonblock_mode(out[1]))break;
		
			sv[0] = (int)out[0];
			sv[1] = (int)out[1];

			return 0;
		} while (0);

		::closesocket(out[0]);
		::closesocket(out[1]);

		return -1;
#else
		if(-1==::socketpair( AF_UNIX,SOCK_DGRAM,0,out ) )return -1;
			
		if(!_socket_set_nonblock_mode(out[0]))return -1;
		if(!_socket_set_nonblock_mode(out[1]))return -1;

		sv[0] = (int)out[0];
		sv[1] = (int)out[1];
		return 0;
#endif
	}

	void socket_native::close_udp_pair(int sv[2])
	{
		if(-1!=sv[0])::closesocket((SOCKET)sv[0]);
		if(-1!=sv[1])::closesocket((SOCKET)sv[1]);

		sv[0]=-1;
		sv[1]=-1;
	}
};