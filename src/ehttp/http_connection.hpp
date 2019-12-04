#pragma once
#include "../ebase/string.hpp"
#include "../eio/socket_io.hpp"

namespace ehttp
{

    class http_connection
    {
    public:
        http_connection();
        ~http_connection();

        bool                open_request(const char* method,const char* url,const char* body,int body_length);
        void                end_request();

    private:
    };

};