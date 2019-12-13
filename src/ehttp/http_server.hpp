#pragma once
#include "../eio/socket_io.hpp"
#include "../eio/socket_native.hpp"
namespace ehttp
{
    class http_server:public ebase::ref_class<>
    {
    public:
        http_server(ebase::executor* event_executor=0);
        ~http_server();

        void            set_event_executor( ebase::executor* event_executor );
        bool            listen(const eio::socket_address& address );
        bool            listen( int port = 80,bool ipv6=false );

        ebase::event_emitter   on_error;
        ebase::event_emitter   on_request;

    private:
        ebase::executor*        _event_executor;
        eio::socket_native_ptr   _listen_socket;
        ebase::ref_list         _pendding_list;

        void			        on_acceptable(ebase::ref_class_i* fire_from_handle);
    };

};