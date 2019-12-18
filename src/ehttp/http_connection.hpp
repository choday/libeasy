#pragma once
#include "../ebase/string.hpp"
#include "../ebase/ref_class.hpp"
#include "../ebase/ref_list.hpp"
#include "http_socket.hpp"

namespace ehttp
{

    class http_connection
    {
    public:
        http_connection();
        ~http_connection();

    };

    typedef ebase::ref_ptr<http_connection> http_connection_ptr;
};