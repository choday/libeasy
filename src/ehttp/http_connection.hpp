#pragma once
#include "../ebase/string.hpp"
#include "../ebase/ref_class.hpp"
#include "../ebase/ref_list.hpp"
#include "../eio/socket_io.hpp"
#include "../ebase/zlib_wrap.hpp"
#include "../ebase/stream.hpp"

#include "http_response.hpp"
#include "http_request.hpp"
/*
分几种情况 

1. 如果有content-length,接收数据长度为content-length,在接收过程中连接断开，数据截断
2. http 1.1 如果不是keep-alive,必须在content-length与chunked间二选一，如果一个都没有，则协议退化为1.0
3. 有content-length时，我们直接处理body数据，不再经过http协议
4. 有chunked时，需要经过http协议确认body长度
5. http 1.0 不支持chunked,不支持keepalive
*/
namespace ehttp
{

    class http_connection:public eio::socket_io_filter,public ebase::ref_list::entry,public http_protocol::http_protocol_callback
    {
    public:
        http_connection();
        ~http_connection();

        http_request            request;
        http_response           response;

        bool                    open_request(const ebase::string& method,const ebase::string& url );

        virtual bool            open(const ebase::string& host=ebase::string(),const ebase::string& port_or_service=ebase::string()) override;
        

        virtual int             write_buffer( const ebase::buffer& data ) override;
        virtual int             read_buffer( ebase::buffer& data ) override;
    private:
		virtual void			notify_error(ref_class_i* fire_from_handle) override;
		virtual void			notify_opened(ref_class_i* fire_from_handle) override;
		virtual void			notify_closed(ref_class_i* fire_from_handle) override;

		virtual void			notify_readable(ref_class_i* fire_from_handle) override;
		virtual void			notify_writeable(ref_class_i* fire_from_handle) override;

        bool                    do_fetch_once(ebase::buffer& out_buffer);
        int                     do_flush();
    private:
        ebase::buffer           _cache_in;
        ebase::buffer           _cache_out;
        bool                    _is_server;
        http_protocol*          _current_http_parser;
        bool                    _is_http_opened;
        ebase::zlib_wrap        _deflate_method;
        int                     _max_http_read_cache;

        ebase::buffer           _http_cache_data;
        ebase::buffer*          _http_fetch_once_body_cache;

        virtual bool on_http_protocol_headers_complete() override;
        virtual void on_http_protocol_body(const char* data,size_t len)override;
        virtual void on_http_protocol_complete()override;
    };

    typedef ebase::ref_ptr<http_connection> http_connection_ptr;
};