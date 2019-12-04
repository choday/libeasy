#pragma once
#include "../eio/socket_native.hpp"

class connect_multi:public ebase::ref_class<>
{
public:
	connect_multi(ebase::executor* event_executor=0);
	~connect_multi();

	void start( eio::socket_address& remote_address,int test_connection = 10240 );
private:
	ebase::executor* _event_executor;
	int				_test_connection;
	int				current_connections;
	eio::socket_address _remote_address;
	static const int		connet_count = 1;

	void on_read(ref_class_i* handle);
	void on_error(ref_class_i* handle);
	void on_connect(ref_class_i* handle);
	void on_closed(ref_class_i* handle);

};