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

        ebase::string           upgrade_protocol;//https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Protocol_upgrade_mechanism
        ebase::string           content_encoding;
        uint64_t                content_length;
        ebase::string           connection;

        ebase::string_array     headers;
        /**
        @brief Ìí¼ÓhttpÍ·
        @param[in] name http header field
        @param[in] value http header value
        */
        virtual bool            add_header(const ebase::string& value);
        bool                    add_header(const ebase::string& name,const ebase::string& value);

        void                    reset();

    protected:

        void                    make_headers(ebase::buffer& in_out);
        void                    make_version(ebase::buffer& in_out);

        const char* find_header_value(const char* header);
    private:
        virtual void                    on_http_parser_add_header( const ebase::string& value ) override;
    };
};