#pragma once
#include "../ebase/ref_class.hpp"
#include "../ebase/buffer.hpp"
#include "../ebase/event_emitter.hpp"

namespace eio
{

    class io_method:public ebase::ref_class_i
    {
    public:
        io_method();

		ebase::event_emitter				on_error;///< io出错,调用get_error_code,get_error_message获取更多信息
		ebase::event_emitter				on_closed;///< io对象被关闭
		ebase::event_emitter				on_readable;///< 可进行读操作
		ebase::event_emitter				on_writeable;///< 可写

        ///为所有事件设置执行器ebase::executor
        virtual void            set_event_executor( ebase::executor* event_executor );

        
        virtual bool            is_opened() = 0;///< io是否打开
        virtual int             get_error_code() const = 0;///< 获取错误码
        virtual const char*     get_error_message() const =0;///< 获取错误码描述字符串

/**
@brief 关闭对象

@params[in] delay 是否延迟关闭
@note 如果参数delay=true，那么会等到所有数据处理完闭才会真正关闭,否则立即关闭并丢弃所有数据
*/
        virtual void            close(bool delay=true) = 0;
/**
@brief 读取数据 

@param[out] data 缓冲区
@param[in] len 缓冲区长度
@retval >0 返回数据长度
@retval 0 无数据可读
@retval <0 出错，以后不再可读,详情调用get_error_code
*/
        virtual int             read(void* data,int len) = 0;
/**
@brief 写入数据

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
@brief 读取数据到ebase::buffer对象
@details 从下层直接弹出数据,如果下层没有实现，默认调用read
@param[out] data 返回的数据缓冲区，如果不提前分配，则底层自动分配
@return 返回弹出数据长度,即data.size();出错返回-1,不可写返回0
@note 此函数缓冲区操作交给底层实现者，调用者无法控制缓存大小,如果传入的data.capacity()==0,底层调用data.alloc()分配内存
*/
        virtual int            read_buffer( ebase::buffer& data );
/**
@brief 写入数据 
@details 把一个数据块缓冲区直接送给下层处理,减少数据复制,如果下层没有处理，默认调用write
@param[in] data
@return 返回数据长度,即data.size();出错返回-1,不可写返回0
@note 此函数缓冲区操作交给底层实现者，调用者无法控制缓存大小
*/
        virtual int            write_buffer( const ebase::buffer& data );


    };

};