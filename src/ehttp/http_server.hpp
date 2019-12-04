#pragma once
#include "../eio/socket_io.hpp"

namespace ehttp
{

    class http_server
    {
    public:

        bool            listen(const eio::socket_address& address );

        ebase::event_emitter   on_error;
        ebase::event_emitter   on_request;
    };

};