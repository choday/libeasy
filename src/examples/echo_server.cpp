#include "echo_server.hpp"
#include <stdio.h>

echo_server::echo_server(ebase::executor* event_executor)
{
	_socket = eio::socket_native::create_instance( event_executor );
}

echo_server::~echo_server()
{

}

bool echo_server::start(eio::socket_address& address,bool is_tcp)
{
	bool result;
	if(is_tcp)
	{
		result = _socket->listen( address );
		_socket->on_readable.set_function( &echo_server::on_accept,this );
	}else
	{
		result = _socket->bind(address);
	}

	eio::socket_address addr=_socket->get_local_address();
	printf((is_tcp?"listen on:%s\n":"bind on:%s\n"),addr.to_string().c_str() );
	return result;
}

void echo_server::on_accept(ref_class_i* handle)
{
	eio::socket_native_ptr ptr = _socket->accept();

	while(ptr)
	{
		
		ptr->on_readable.set_function( &echo_server::on_read,this );
		ptr->on_closed.set_function( &echo_server::on_closed,this );
		ptr->on_error.set_function( &echo_server::on_error,this );

		eio::socket_address addr=ptr->get_remote_address();
		printf("accept:%s\n",addr.to_string().c_str() );

		ptr = _socket->accept();
	}
}

void echo_server::on_read(ref_class_i* handle)
{
	eio::socket_native* p = (eio::socket_native*)handle;

	ebase::buffer b;
	while( p->read_buffer(b) )
	{
		p->write_buffer(b);
	}
}

void echo_server::on_closed(ref_class_i* handle)
{
	eio::socket_native* p = (eio::socket_native*)handle;
	printf("server on_closed\n");
}
void echo_server::on_error(ref_class_i* handle)
{
	eio::socket_native* p = (eio::socket_native*)handle;

	//printf("server on_error:%d %s\n",p->error.code,p->error.message.c_str());
}