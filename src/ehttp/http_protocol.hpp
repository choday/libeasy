#pragma once
#include "http_parser_wrap.hpp"
#include "../ebase/buffer.hpp"

namespace ehttp
{
    class http_protocol:public http_parser_wrap
    {
    public:
        http_protocol();
        ~http_protocol();

        virtual ebase::buffer   make_buffer() = 0;

        int                     major_ver;
        int                     minor_ver;
        bool                    chunked;//Transfer-Encoding: chunked
        bool                    keep_alive;//keepalive tcp for http 1.1
        bool                    upgrade;

        ebase::string           upgrade_protocol;//https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Protocol_upgrade_mechanism
        ebase::string           content_encoding;
        ebase::string           connection;
        ebase::string_array     headers;
        /**
        @brief Ìí¼ÓhttpÍ·
        @param[in] name http header field
        @param[in] value http header value
        */
        virtual void            add_header(const ebase::string& value);
        void                    add_header(const ebase::string& name,const ebase::string& value);

        void                    reset();

        void                    make_headers(ebase::buffer& in_out);
        void                    make_version(ebase::buffer& in_out);
        
        bool                    has_content_length();
        uint64_t                get_content_length();
        bool                    is_end();

        class http_protocol_callback
        {
        public:
            virtual bool on_http_protocol_headers_complete(){return true;}
            virtual void on_http_protocol_body(const char* data,size_t len){}
            virtual void on_http_protocol_chunk_header(){}
            virtual void on_http_protocol_chunk_complete(){}
            virtual void on_http_protocol_complete(){}
        };

        void inline set_callback(http_protocol_callback* c){_http_protocol_callback=c;}
    protected:

        http_protocol_callback*     _http_protocol_callback;

        const char* find_header_value(const char* header);
    private:

        virtual void                    on_http_parser_add_header( const ebase::string& value ) override;
        virtual bool                    on_http_parser_headers_complete()override;//return false to skip body

        virtual inline void            on_http_parser_body( const char* data,size_t len ){if(_http_protocol_callback)_http_protocol_callback->on_http_protocol_body(data,len);}
        virtual inline void            on_http_parser_chunk_header(){if(_http_protocol_callback)_http_protocol_callback->on_http_protocol_chunk_header();}
        virtual inline void            on_http_parser_chunk_complete(){if(_http_protocol_callback)_http_protocol_callback->on_http_protocol_chunk_complete();}

        virtual inline void            on_http_parser_end(){if(_http_protocol_callback)_http_protocol_callback->on_http_protocol_complete();}
    };
};