#pragma once
#include "../eio/socket_native.hpp"


class echo_server:public ebase::ref_class<>
{
public:
	echo_server(ebase::executor* event_executor=0);
	~echo_server();

	bool start( eio::socket_address& address,bool is_tcp );
private:
	eio::socket_native_ptr	_socket;
	ref_class_i* p;

	void on_accept(ref_class_i* handle);
	void on_read(ref_class_i* handle);
	void on_closed(ref_class_i* handle);
	void on_error(ref_class_i* handle);
};