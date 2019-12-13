#pragma once
#include "../ebase/string.hpp"
#include "../ebase/buffer.hpp"
#include "http_url.hpp"
#include "http_protocol.hpp"

#ifndef default_user_agent
#define default_user_agent "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36"
#endif

namespace ehttp
{
//Accept-Encoding: br =>Brotli

    class http_request :public http_protocol
    {
    public:
        http_request();
        ~http_request();

        http_url                url;
        ebase::string           method;
        ebase::string           host;//Host: www.baidu.
        ebase::string           user_agent;
        ebase::string           referer;
        ebase::string           accept_encoding;//Accept-Encoding: gzip, deflate, br
        ebase::string           authorization;
        ebase::string           cookie;

        ebase::buffer           body;

        virtual void            add_header(const ebase::string& value) override;

        ebase::buffer           make_buffer();
    private:
        
    };

};