#include "connect_multi.hpp"
#include <stdio.h>

connect_multi::connect_multi(ebase::executor* event_executor):_event_executor(event_executor)
{
current_connections=0;
}

connect_multi::~connect_multi()
{

}

void connect_multi::start(eio::socket_address& remote_address,int test_connection /*= 1024 */)
{
	this->_test_connection = test_connection;
	this->_remote_address = remote_address;

	for(int i  = 0;i<connet_count;++i)
	{
		eio::socket_native_ptr	_socket = eio::socket_native::create_instance(this->_event_executor);

		_socket->on_readable.set_function( &connect_multi::on_read,this );
		_socket->on_closed.set_function( &connect_multi::on_closed,this );
		_socket->on_opened.set_function( &connect_multi::on_connect,this );
		_socket->on_error.set_function( &connect_multi::on_error,this );

		if(!_socket->open( this->_remote_address ))
		{
			printf("connect fail:%s\n",_socket->get_error_message() );
			break;
		}
	}
}	

void connect_multi::on_read(ref_class_i* handle)
{
	eio::socket_native* p = (eio::socket_native*)handle;

	ebase::buffer b;

	while( p->read_buffer(b)>0 )
	{

	}
	eio::socket_address addr=p->get_local_address();
	if(addr.size())printf("recv local:%d %s\n",p->get_handle(),addr.to_string().c_str() );
	p->close();

	if(current_connections++>_test_connection)return;

		eio::socket_native_ptr	_socket = eio::socket_native::create_instance(this->_event_executor);

		_socket->on_readable.set_function( &connect_multi::on_read,this );
		_socket->on_closed.set_function( &connect_multi::on_closed,this );
		_socket->on_opened.set_function( &connect_multi::on_connect,this );
		_socket->on_error.set_function( &connect_multi::on_error,this );

		if(!_socket->open( this->_remote_address ))
		{
			printf("connect fail:%s\n",_socket->get_error_message());
		}else
		{
			addr=_socket->get_local_address();
			printf("connectting:%d %s\n",_socket->get_handle(),addr.to_string().c_str() );
		}
}

void connect_multi::on_error(ref_class_i* handle)
{
		eio::socket_native* p = (eio::socket_native*)handle;

		if(64!=p->get_error_code())
		{
			printf("client on_error:%d %d %s\n",p->get_handle(),p->get_error_code(),p->get_error_message());
		}
}

void connect_multi::on_connect(ref_class_i* handle)
{
	ebase::buffer b;
	eio::socket_native* p = (eio::socket_native*)handle;

	if(p->is_opened())
	{

		b.assign("test",4);
		p->write_buffer(b);

		eio::socket_address addr=p->get_local_address();
		printf("connect local:%d %s %d\n",p->get_handle(),addr.to_string().c_str(),current_connections );
	}
}

void connect_multi::on_closed(ref_class_i* handle)
{
	eio::socket_native* p = (eio::socket_native*)handle;
	printf("client on_closed:%d\n",p->get_handle());
}

