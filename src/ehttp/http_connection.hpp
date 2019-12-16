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
�ּ������ 

1. �����content-length,�������ݳ���Ϊcontent-length,�ڽ��չ��������ӶϿ������ݽض�
2. http 1.1 �������keep-alive,������content-length��chunked���ѡһ�����һ����û�У���Э���˻�Ϊ1.0
3. ��content-lengthʱ������ֱ�Ӵ���body���ݣ����پ���httpЭ��
4. ��chunkedʱ����Ҫ����httpЭ��ȷ��body����
5. http 1.0 ��֧��chunked,��֧��keepalive
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