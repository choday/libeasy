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
        ///_head_cache_in���ֵ,���http header�������ֵ���������
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


        int                     do_fetch_chunk_header();//����0,��Ҫ�ٴε��ã�����-1,�������ش���0(�Ѵ����ֽ�����),�ɹ�
        int                     do_fetch_chunk_end();//����0,��Ҫ�ٴε��ã�����-1,�������ش���0(�Ѵ����ֽ�����),�ɹ�
        
        int                     do_read_body(char* data,int len);
        int                     do_read_data(char* data,int len);//��_head_cache_in���ߵײ��ȡ����

        int                     do_parse_header();//����0,��Ҫ�ٴε��ã�����-1,�������ش���0,�ɹ�
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
            read_body_need_header,//��û��body,��Ҫ����http header
            read_body_eof,//body ����Ϊ0�����Ѷ���
            read_body_to_content_length,//body ���Ȳ�Ϊ0,����Ҫ������ȡthis->content_nread����
            read_body_need_thunked_header,//��Ҫ����chunked header
            read_body_to_thunked_length,//��Ҫ��ȡthis->content_nread����
            read_body_to_connection_close,//��Ҫ��ȡ��ֱ�����ӽ���
        }                       _read_body_type;

        virtual bool on_http_protocol_headers_complete() override;
        virtual void on_http_protocol_chunk_header();
        virtual void on_http_protocol_chunk_complete();
        virtual void on_http_protocol_complete()override;
    };

    typedef ebase::ref_ptr<http_socket> http_socket_ptr;
};