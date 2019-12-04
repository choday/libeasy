#include "../ebase/thread_loop.hpp"
#include "../eio/socket.hpp"
#include "../examples/echo_server.hpp"

int main()
{
	ebase::thread_loop loop;



	ebase::ref_ptr<echo_server>	_echo_server=new echo_server(&loop);
	eio::socket_address address;

	address.set_port(90);
	address.set_family(false);

	_echo_server->start(address,true);

	loop.run();	
};