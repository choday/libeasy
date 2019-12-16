#pragma once
#include "../eio/socket_io.hpp"
#include "../eio/socket_native.hpp"
#include "../ebase/string.hpp"
#include "http_virtual_server.hpp"

namespace ehttp
{
    class http_server:public http_virtual_server
    {
    public:
        http_server(ebase::executor* event_executor=0);
        ~http_server();

        bool                    listen(const eio::socket_address& address );
        bool                    listen( int port = 80,bool ipv6=false );

        bool                    add_virutal_server(const ebase::string& host,http_virtual_server* s );

    protected:
        virtual void            on_http_upgrade(http_socket* s);
        virtual void            on_http_request(http_socket* s);
    private:
        ebase::executor*         _event_executor;
        eio::socket_native_ptr   _listen_socket;
        ebase::ref_list          _pendding_list;
        ebase::ref_tree          _virutal_server_map;

        void			         on_acceptable(ebase::ref_class_i* fire_from_handle);
        void			         on_opened(ebase::ref_class_i* fire_from_handle);
        void			         on_error(ebase::ref_class_i* fire_from_handle);
    };

};