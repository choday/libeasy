#pragma once
#include "../ebase/ref_class.hpp"
#include "../ebase/buffer.hpp"
#include "../ebase/error.hpp"
#include "../ebase/executor.hpp"
#include "../ebase/atomic.hpp"
#include "../ebase/allocator.hpp"
#include "../ebase/event_emitter.hpp"
#include "../ebase/ref_list.hpp"
#include "socket_io.hpp"
#include "name_resolver.hpp"

#ifdef _WIN32
#include <BaseTsd.h>
#else
#include <netdb.h>   
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

namespace eio
{
	#define socket_default_read_buffer_size 1600

	class socket_native;
	typedef ebase::ref_ptr<socket_native>	socket_native_ptr;

    class socket_native:public ebase::ref_class<socket_io>,public ebase::ref_list::entry
	{
	public:
		socket_native(ebase::executor* event_executor=0);
		virtual ~socket_native();

		enum 
		{
            socket_api_default  = 0,
			socket_api_platform = 1,//默认 iocp,epoll,kqueue
			socket_api_poll,//poll
			//socket_api_select,//select only,最大管理数量,win32(64),linux(1024)
		};

        static void                     set_default_api_type(int socket_api_type=socket_api_platform);
		static socket_native_ptr		create_instance(ebase::executor* event_executor=0,int socket_api_type = socket_api_default );
		
		virtual bool					init(int af,int type,int protocol);
		void							set_default_read_buffer_size(int capcity=socket_default_read_buffer_size );

		virtual bool					bind( const socket_address& address );
		virtual bool					listen(const socket_address& address,int backlog=20);
        virtual socket_native_ptr		accept();
		virtual socket_native_ptr		attach_accept_handle(SOCKET s);
        virtual int                     sendto( const ebase::buffer& data,const socket_address& to_address );
        virtual int                     recvfrom( ebase::buffer& data,socket_address* from_address=0 );

//socket_io start
		virtual bool			open( const ebase::string& host,const ebase::string& port_or_service ) override;
		virtual bool			open(const socket_address& address ) override;
        virtual bool            is_opened() override;
        virtual void            close(bool delay=true) override;
        virtual int             write( const void* data,int len ) override;
        virtual int             read( void* data,int len ) override;
        virtual int             get_nread_size() const override;
        virtual int             get_error_code() const override;
        virtual const char*     get_error_message() const override;
//socket_io end

		bool					is_invalid_handle() const;

		bool					is_writeable();
		bool					is_readable();
		bool					is_stream();
		SOCKET					get_handle() const;
		
		//socket 关闭以后，下面两个函数不可用
		socket_address			get_local_address() const;
		socket_address			get_remote_address() const;

		bool					set_keepalive( bool enable,int idle_seconds = 60,int interval_seconds = 5,int count = 3);
		bool					set_send_buffer_size(int maxbuffersize);
		bool					set_recv_buffer_size(int maxbuffersize);
		bool					set_nonblock_mode();

		int						get_socket_error_code();
		//int					get_connect_time();
		int						get_socket_type();
		static int				get_last_error();

        static int	            socket_udp_pair( int sv[2] );//这里与linux有区别，输出 两个socket都是nonblock模式,简单的udp socket对
		static void	            close_udp_pair(int sv[2]);

    protected:
		virtual	bool		    native_enter_listener() = 0;
		virtual	void		    native_leave_listener() = 0;
        virtual void            native_io_flags_changed() = 0;
        virtual void		    native_want_read();
        virtual void		    native_want_write();

        //native io,成功返回1，出错返回-1,阻塞返回0
		virtual	SOCKET		    native_create_socket(int af,int type,int protocol);
        virtual int			    native_connect(const socket_address& address );
        virtual int             native_send( const void* data,int len );
        virtual int             native_recv( void* data,int len );
        virtual int             native_sendto( const ebase::buffer& data,const socket_address& to_address );
        virtual int             native_recvfrom( ebase::buffer& data,socket_address* from_address=0 );

	protected:
		ebase::executor*		_event_executor;
		int						_default_read_buffer_size;//参数，接收udp数据包时，最大大小
		ebase::atomic_flags		_status;
		SOCKET					_handle;
		int						_family;
        ebase::error			_error;

		void					process_error(bool self_socket_error);//self_socket_error是否为socket自身出错，否则 为外部错误，外部错误不需要关闭socket,可继续运行
		void					process_closed();

		void					process_readable();
		void					process_writeable();//设置status_flags_connectting后，将调用on_connected，否则 调用notify_writeable

		void			notify_error(ref_class_i* fire_from_handle);
		void			notify_opened(ref_class_i* fire_from_handle);
		void			notify_closed(ref_class_i* fire_from_handle);

		void			notify_readable(ref_class_i* fire_from_handle);
		void			notify_writeable(ref_class_i* fire_from_handle);

		enum
		{
			status_flags_readable = 1,
			status_flags_writeable = (1<<1),
			status_flags_connected = (1<<2),
			status_flags_listened=	(1<<3),
			status_flags_bind	 = (1<<4),
			status_flags_stream=	(1<<5),
			status_flags_socket_valid = (1<<6),
			status_flags_connectting = (1<<7),//将对notify_error和notify_writeable产生影响
            status_flags_close_after_send = (1<<8),

			socket_status_flags_want_read = (1<<9),
			socket_status_flags_want_write = (1<<10),

			status_flags_user_bits = 16,
		};

		long					set_flags(long mask);
		bool					test_flags(long mask);
		long					clear_flags(long mask);
	private:
		void	                resolver_and_connect(io_request* request);
		bool	                process_resolver_and_connect();//return false to continue
		ebase::ref_ptr<name_resolver::request>	resolver_request_ptr;
	};
};