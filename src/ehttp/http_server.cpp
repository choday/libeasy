#include "http_server.hpp"
#include "http_connection.hpp"
#include "http_socket.hpp"
#include "../eio/ssl/ssl_socket.hpp"
#include <stdio.h>
namespace ehttp
{

    http_server::http_server(ebase::executor* event_executor):http_virtual_server(event_executor),_event_executor(event_executor)
    {

    }

    http_server::~http_server()
    {

    }

    bool http_server::listen(const eio::socket_address& address)
    {
        if(!_listen_socket.valid())_listen_socket=eio::socket_native::create_instance(_event_executor);

        _listen_socket->on_readable.set_function( &http_server::on_acceptable,this );

        return _listen_socket->listen( address );
    }

    bool http_server::listen(int port /*= 80 */,bool ipv6)
    {
        eio::socket_address bind_addr;

        bind_addr.set_family( ipv6 );
        bind_addr.set_port(port);

        return listen( bind_addr );
    }

    bool http_server::add_virutal_server(const ebase::string& host,http_virtual_server* s)
    {
        //if(_ssl_context.valid()!=s->_ssl_context.valid())return false;

        s->_host = host;
        return _virutal_server_map.insert_equal( s );
    }


    bool http_server::do_accept()
    {

        eio::socket_native_ptr new_socket = _listen_socket->accept();
        if(!new_socket.valid())return false;

        http_socket_ptr        phttp_socket = new http_socket();

        phttp_socket->on_opened.set_function( &http_server::on_opened,this );
        phttp_socket->on_error.set_function( &http_server::on_error,this );
        if(_ssl_context.valid())
        {
            ebase::ref_ptr<eio::ssl_socket> ssl = _ssl_context->create_ssl_socket();
            

            ssl->attach_socket_io(new_socket);
            if(_host.length())ssl->ssl_set_host_name( _host.c_str() );
            ssl->init(true);
            phttp_socket->attach_socket_io(ssl);
        }else
        {
            phttp_socket->attach_socket_io(new_socket);
        }

        phttp_socket->init(true);

        bool result = _pendding_list.push_back( phttp_socket.get() );
        assert(result);
        printf("accept\n");
        return true;
    }

    void http_server::on_acceptable(ebase::ref_class_i* fire_from_handle)
    {
        while(do_accept()){};
    }

    void http_server::on_opened(ebase::ref_class_i* fire_from_handle)
    {
        http_socket* _http_socket = (http_socket*)fire_from_handle;
        _pendding_list.remove(_http_socket);

        if(_http_socket->request.upgrade)
        {
            on_http_upgrade(_http_socket);
        }else
        {
            on_http_request(_http_socket);
        }
    }

    void http_server::on_error(ebase::ref_class_i* fire_from_handle)
    {
        http_socket* _http_socket = (http_socket*)fire_from_handle;
    }


    void http_server::on_http_request(http_socket* _http_socket)
    {
        _http_socket->send_response("ok");
        //http_virtual_server::add_request( _http_socket );
    }

    void http_server::on_http_upgrade(http_socket* _http_socket)
    {
        _http_socket->send_response(HTTP_STATUS_BAD_REQUEST);
    }
};