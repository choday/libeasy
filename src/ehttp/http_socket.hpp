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
    class http_socket:public eio::socket_io_filter,public http_protocol::http_protocol_callback,public ebase::ref_list::entry
    {
    public:
        http_socket();
        ~http_socket();

        http_request            request;
        http_response           response;
        ///_head_cache_in最大值,如果http header大于这个值，将会出错
        int                     max_head_cache_size;
        int                     max_head_line_size;

        virtual void            init(bool is_server=true);

        virtual bool            open();
        virtual bool            open(const ebase::string& host,const ebase::string& port_or_service) override;
        virtual bool			open(const eio::socket_address& address ) override;

        virtual int             write( const void* data,int len ) override;
        virtual int             read( void* data,int len ) override;

        virtual int             get_nread_size() const override;

        int                     send_response(int status_code=200,const ebase::string& status_msg="");
        int                     send_response(const ebase::string& data);

        int                     write_http_body( const void* data,int len );
        int                     read_http_body( void* data,int len );
    protected:

		virtual void			notify_error(ref_class_i* fire_from_handle) override;
		virtual void			notify_opened(ref_class_i* fire_from_handle) override;
		virtual void			notify_closed(ref_class_i* fire_from_handle) override;

		virtual void			notify_readable(ref_class_i* fire_from_handle) override;
		virtual void			notify_writeable(ref_class_i* fire_from_handle) override;


        int                     do_fetch_chunk_header();//返回0,需要再次调用，返回-1,出错，返回大于0(已处理字节数量),成功
        int                     do_fetch_chunk_end();//返回0,需要再次调用，返回-1,出错，返回大于0(已处理字节数量),成功
        
        int                     do_read_body(char* data,int len);
        int                     do_read_data(char* data,int len);//从_head_cache_in或者底层读取数据

        int                     do_parse_header();//返回0,需要再次调用，返回-1,出错，返回大于0,成功
        int                     do_head_cache_in();
    private:
        ebase::buffer           _head_cache_in;
        int                     _head_cache_in_offset;
        int                     _head_cache_in_line_end;
        int                     _total_head_size;
        ebase::buffer           _head_cache_out;

        bool                    _is_server;
        http_protocol*          _current_http_parser;
        uint64_t                content_nread;
        bool                    _is_http_opened;

        enum
        {
            read_body_need_header,//还没有body,需要解析http header
            read_body_eof,//body 长度为0或者已读完
            read_body_to_content_length,//body 长度不为0,还需要继续读取this->content_nread长度
            read_body_need_thunked_header,//需要处理chunked header
            read_body_to_thunked_length,//需要读取this->content_nread长度
            read_body_to_connection_close,//需要读取，直到连接结束
        }                       _read_body_type;

        virtual bool on_http_protocol_headers_complete() override;
        virtual void on_http_protocol_chunk_header();
        virtual void on_http_protocol_chunk_complete();
        virtual void on_http_protocol_complete()override;
    };

    typedef ebase::ref_ptr<http_socket> http_socket_ptr;
};