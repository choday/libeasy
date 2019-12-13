#include "http_server.hpp"
#include "http_connection.hpp"

namespace ehttp
{

    http_server::http_server(ebase::executor* event_executor):_event_executor(event_executor)
    {

    }

    http_server::~http_server()
    {

    }

    void http_server::set_event_executor(ebase::executor* event_executor)
    {
        _event_executor=event_executor;
        if(_listen_socket.valid())_listen_socket->set_event_executor(event_executor);
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

    void http_server::on_acceptable(ebase::ref_class_i* fire_from_handle)
    {
        eio::socket_native_ptr new_socket = _listen_socket->accept();

        http_connection* p = new http_connection();

        _pendding_list.push_back(p);

        p->attach_socket_io( new_socket );
        
    }

};