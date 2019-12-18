#pragma once
#include "../ebase/string.hpp"
#include "../ebase/ref_class.hpp"
#include "../ebase/ref_list.hpp"
#include "../eio/socket_io.hpp"
#include "../ebase/zlib_wrap.hpp"
#include "../ebase/stream.hpp"
#include "../ebase/ref_list.hpp"
#include "http_response.hpp"
#include "http_request.hpp"

//https://www.php.cn/manual/view/35537.html
//https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Protocol_upgrade_mechanism
namespace ehttp
{
    /**
    @brief http socket io
    @details 实现http协议io

    @code
        简单用法
        http_socket_ptr _http_socket=new http_socket();

        _http_socket->request.url.parse("https://cloud.tencent.com/developer/section/1189886");

        ... wait for event:_http_socket->on_open


        _http_socket->read_buffer();
    @endcode
    */
    class http_socket:public eio::socket_io_filter,public ebase::ref_list::entry
    {
    public:
        http_socket();
        ~http_socket();

        http_request            request;
        http_response           response;

        virtual void            init(bool is_server=true);

        virtual bool            open();
        virtual bool            open(const ebase::string& host,const ebase::string& port_or_service) override;
        virtual bool			open(const eio::socket_address& address ) override;
        virtual void            close(bool delay=true) override;

        virtual int             write( const void* data,int len ) override;
        virtual int             read( void* data,int len ) override;

        virtual int             get_nread_size() const override;
        virtual int             get_error_code() const override ;
        virtual const char*     get_error_message() const override;

        int                     begin_write();
        bool                    end_write();

        int                     send_response(int status_code=200,const ebase::string& status_msg="");
        int                     send_response(const ebase::string& data);//begin_write+write+end_write
    protected:
		virtual void			notify_error(ref_class_i* fire_from_handle) override;
		virtual void			notify_opened(ref_class_i* fire_from_handle) override;
		virtual void			notify_closed(ref_class_i* fire_from_handle) override;

		virtual void			notify_readable(ref_class_i* fire_from_handle) override;
		virtual void			notify_writeable(ref_class_i* fire_from_handle) override;

        int                     write_http_data( const void* data,int len );
        int                     read_http_data( void* data,int len );
    private:
        ebase::buffer           _head_cache_out;
        ebase::buffer           _head_cache_in;
        int                     _head_cache_in_offset;
        bool                    _is_write_end;

        bool                    _is_server;
        http_protocol*          _current_http_parser;
        http_protocol*          _current_http_writer;

        int                     do_skip_chunked();

        int                     do_read_http_body(char* data,int len);
        int                     do_read_socket_data(char* data,int len);//从_head_cache_in或者底层读取数据
        int                     do_write_http_chunk( const void* data,int len );
    };

    typedef ebase::ref_ptr<http_socket> http_socket_ptr;
};