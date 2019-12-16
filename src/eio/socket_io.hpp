#pragma once

#include "../ebase/executor.hpp"

#include "../ebase/string.hpp"

#include "socket_address.hpp"
#include "io_method.hpp"
namespace eio
{
    class socket_io:public io_method
    {
    public:
 
        ///数据流成功打开
		ebase::event_emitter				on_opened;


        ///为所有事件设置执行器ebase::executor
        virtual void            set_event_executor( ebase::executor* event_executor ) override;
        virtual void            clear_all_event();

/**@brief 打开一个数据流,底层调用tcp socket connect系统函数实现

@param[in] host 目标域名或者ip字符串
@param[in] port_or_service 目标port或者service字符串，如"80"、"http"等
@param[in] address 目标inet sockaddr
@return 成功返回true
@note 返回true,意味着成功发起connect,连接成功,on_opened事件将会被触发
*/
		virtual bool			open( const ebase::string& host,const ebase::string& port_or_service ) =0;
		virtual bool			open(const socket_address& address ) =0;

/**
@brief 查看读缓冲区里面的数据长度

*/
        virtual int             get_nread_size() const =0;
    };

    typedef ebase::ref_ptr<socket_io>   socket_io_ptr;

    class socket_io_filter:public ebase::ref_class<socket_io>
    {
    public:
        socket_io_filter();
        ~socket_io_filter();
        
        virtual void            attach_socket_io(socket_io* next);
        socket_io*              get_next_socket_io();

        virtual void            set_event_executor( ebase::executor* event_executor ) override;

		virtual bool			open( const ebase::string& host,const ebase::string& port_or_service ) override;
		virtual bool			open(const socket_address& address ) override;
        virtual bool            is_opened() override;
        virtual void            close(bool delay=true) override;

        virtual int             read(void* data,int len) override;
        virtual int             write(const void* data,int len) override;

        virtual int             get_nread_size() const override;
        virtual int             get_error_code() const override ;
        virtual const char*     get_error_message() const override;
    protected:
		virtual void			notify_opened(ref_class_i* fire_from_handle);

		virtual void			notify_error(ref_class_i* fire_from_handle);
		virtual void			notify_closed(ref_class_i* fire_from_handle);
		virtual void			notify_readable(ref_class_i* fire_from_handle);
		virtual void			notify_writeable(ref_class_i* fire_from_handle);

        ebase::ref_ptr<socket_io>   _next_socket_io;
        ebase::executor*            _event_executor;
    };


};