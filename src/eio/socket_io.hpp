#pragma once
#include "../ebase/ref_class.hpp"
#include "../ebase/executor.hpp"
#include "../ebase/event_emitter.hpp"
#include "../ebase/string.hpp"
#include "../ebase/buffer.hpp"
#include "socket_address.hpp"
namespace eio
{
    class socket_io:public ebase::ref_class_i
    {
    public:
 
        ///tcp数据流成功打开
		ebase::event_emitter				on_opened;
        ///socket出错
		ebase::event_emitter				on_error;
        ///tcp数据流被关闭
		ebase::event_emitter				on_closed;

        ///可进行读操作,或者可进行accept操作
		ebase::event_emitter				on_readable;
        ///可写
		ebase::event_emitter				on_writeable;

        ///为所有事件设置执行器ebase::executor
        virtual void            set_event_executor( ebase::executor* event_executor );
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
        ///tcp数据流是否打开
        virtual bool            is_opened() = 0;
/**
@brief 关闭套接字 

@params[in] delay 是否延迟关闭
@note 如果参数delay=true，那么会等到所有数据发送完闭才会真正关闭socket,否则立即关闭socket丢弃所有数据
*/
        virtual void            close(bool delay=true) = 0;
/**
@brief 读取套接字中的数据 

@param[out] data 缓冲区
@param[in] len 缓冲区长度
@retval >0 返回数据长度
@retval 0 无数据可读
@retval <0 出错，以后不再可读,详情调用get_error_code
*/
        virtual int             read(void* data,int len) = 0;
/**
@brief 写入数据到套接字

@param[in] data 缓冲区
@param[in] len 缓冲区长度
@retval >0 返回数据长度
@retval 0 无数据可读
@retval <0 出错，以后不再可写,详情调用get_error_code
@note 对于写入操作，要么全部写入，要么不写入，不存在写入部分这样的操作,所以，返回值如果不是-1,0,则一定是len
@attention 注意，如果传入len=0,那么返回值也会为0
*/
        virtual int             write(const void* data,int len) = 0;
/**
@brief 弹出数据 ,减少数据复制
@details 从底层直接弹出数据
@param[out] data 返回的数据缓冲区，如果不提前分配，则底层自动分配
@return 返回弹出数据长度,即data.size();出错返回-1,不可写返回0
@note 此函数缓冲区操作交给底层实现者，调用者无法控制缓存大小,如果传入的data.capacity()==0,底层调用data.alloc()分配内存
*/
        virtual int            read_buffer( ebase::buffer& data ) = 0;
/**
@brief 压入数据 
@details 把一个数据块缓冲区直接送给下层处理,减少数据复制
@param[in] data
@return 返回数据长度,即data.size();出错返回-1,不可写返回0
@note 此函数缓冲区操作交给底层实现者，调用者无法控制缓存大小
@note 如果返回false,而且get_error_code()==0则socket被关闭,或者传入data.size()=0,调用is_opened查看socket是否被关闭
*/
        virtual int            write_buffer( const ebase::buffer& data ) = 0;
/**
@brief 查看读缓冲区里面的数据长度

*/
        virtual int             get_nread_size() const =0;
        virtual int             get_error_code() const = 0;
        virtual const char*     get_error_message() const =0;
    };

    typedef ebase::ref_ptr<socket_io>   socket_io_ptr;

    class socket_io_wrap:public ebase::ref_class<socket_io>
    {
    public:
        socket_io_wrap();
        ~socket_io_wrap();

        
        virtual void            attach_socket_io(socket_io* next);
        socket_io*              get_next_socket_io();

        virtual void            set_event_executor( ebase::executor* event_executor ) override;

		virtual bool			open( const ebase::string& host,const ebase::string& port_or_service ) override;
		virtual bool			open(const socket_address& address ) override;
        virtual bool            is_opened() override;
        virtual void            close(bool delay=true) override;
          
        virtual int             read(void* data,int len) override;
        virtual int             write(const void* data,int len) override;

        virtual int             read_buffer( ebase::buffer& data ) override;
        virtual int             write_buffer( const ebase::buffer& data ) override;

        virtual int             get_nread_size() const override;
        virtual int             get_error_code() const override ;
        virtual const char*     get_error_message() const override;
    protected:
		virtual void			notify_error(ref_class_i* fire_from_handle);
		virtual void			notify_opened(ref_class_i* fire_from_handle);
		virtual void			notify_closed(ref_class_i* fire_from_handle);

		virtual void			notify_readable(ref_class_i* fire_from_handle);
		virtual void			notify_writeable(ref_class_i* fire_from_handle);

        ebase::ref_ptr<socket_io>   _next_socket_io;
        ebase::executor*            _event_executor;
    };


};