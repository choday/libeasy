#pragma once
#include "../socket_native.hpp"
#include "../../ebase/atomic.hpp"
#include "iocp_win32.hpp"

#include <WinSock2.h>
#include <Mswsock.h>

//WSAIoctl SIO_ADDRESS_LIST_CHANGE,SIO_ROUTING_INTERFACE_CHANGE 可以监控ip地址变化
namespace eio
{
	class socket_win32:public socket_native
	{
	public:
		socket_win32(ebase::executor* event_executor);
		~socket_win32();

       //socket_io
		virtual int 				    write_buffer(const ebase::buffer& buffer) override;
		virtual int                     write( const void* data,int len ) override;


        //socket_native
		virtual socket_native_ptr		accept() override;

 		virtual	bool		            native_enter_listener() override;
		virtual	void		            native_leave_listener() override;
        virtual void                    native_io_flags_changed() override;

        virtual void		            native_want_write() override;

		virtual	SOCKET		            native_create_socket(int af,int type,int protocol) override;
		virtual int					    native_connect( const socket_address& address ) override;

        virtual int     			    sendto(const ebase::buffer& buffer,const socket_address& address) override;

        virtual void		            native_want_read() override;
		virtual int			            native_recv(void* data,int len) override;
		virtual int 			        native_recvfrom(ebase::buffer& data,socket_address* from_address=0) override;

        virtual int                     native_send( const void* data,int len );
        int                             post_write(const ebase::buffer& data);//成功>0,出错<0,上一个post未完成返回0
	private:

		bool					post_accept();

		struct params:public iocp_win32::params
		{
			WSABUF			wsabuffer;
			unsigned long	completed_bytes;
			unsigned long	flags;
			ebase::buffer	hold_buffer;
			socket_address	hold_address;
			int				hold_address_size;
			socket_native_ptr		hold_socket;

			LPWSABUF		init_buffer(const ebase::buffer& b);
			LPWSABUF		init_buffer(int capacity=0);
			void			reset();
		};

		params			_write_params;
		params			_read_params;
		
		class read_cache
		{
		public:
			ebase::buffer		data;
			socket_address		address;
			int					address_size;
			SOCKET				accept_socket;
			bool				has_data;
		};

		ebase::mutex_lock_t<read_cache>	_read_cache;
		bool			fetch_read_cache(ebase::buffer& buffer,socket_address* address);

		void		connect_callback(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params);
		void		accept_callback(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params);
		void		write_callback(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params);
		void		read_callback(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params);
	private:
		static LPFN_CONNECTEX	lpfn_ConnectEx;
		static LPFN_ACCEPTEX	lpfn_AcceptEx;
		static LPFN_DISCONNECTEX	lpfn_DisConnectEx;
		static LPFN_GETACCEPTEXSOCKADDRS	lpfn_GetAcceptExSockaddrs;
		static bool init_winsock32_extension(SOCKET handle);
	};
};