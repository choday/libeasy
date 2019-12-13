#include<iostream>
#include<gtest/gtest.h>
#include <eio/socket_native.hpp>
#include <eio/posix/socket_posix.hpp>
#include <ebase/event_emitter.hpp>

#include <eio/eio.hpp>



TEST(thread_loop,join)
{
	int		memcount = ebase::allocator::get_allocated_count();

    eio::get_network_platform_loop()->start();
    eio::get_network_poll_loop()->start();

    eio::get_network_poll_loop()->abort_all();
    ASSERT_TRUE(eio::get_network_poll_loop()->join());

    eio::get_network_platform_loop()->abort_all();
    ASSERT_TRUE( eio::get_network_platform_loop()->join() );


	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}
void test_socket_connect( int api_level=eio::socket_native::socket_api_platform )
{
	eio::socket_native_ptr socket_handle = eio::socket_native::create_instance( 0,api_level );
	
	ebase::event_emitter_waitor	on_connected;

	on_connected.set_event( socket_handle->on_opened );

	ASSERT_TRUE(socket_handle->open("www.baidu.com","http"));

	ASSERT_TRUE( 0!=on_connected.wait_event(-1) );

	socket_handle->close();
	
	//on_connected.wait(1000);//µÈ´ýsocketÊÍ·Å

	on_connected.clear();

	socket_handle = 0;

}

TEST( socket,connect_baidu )
{
	int		memcount = ebase::allocator::get_allocated_count();

    eio::get_network_platform_loop()->start();
    eio::get_network_poll_loop()->start();


	test_socket_connect(eio::socket_native::socket_api_platform);
	test_socket_connect(eio::socket_native::socket_api_poll);


    eio::get_network_platform_loop()->abort_all();
    eio::get_network_platform_loop()->join();

    eio::get_network_poll_loop()->abort_all();
    eio::get_network_poll_loop()->join();

	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}

void test_socket_accept(int api_level=eio::socket_native::socket_api_platform)
{
    eio::socket_native_ptr listen_handle = eio::socket_native::create_instance( 0,api_level );
    eio::socket_native_ptr connect_handle = eio::socket_native::create_instance( 0,api_level );
    eio::socket_native_ptr accept_handle;

    {
        ebase::event_emitter_waitor	on_acceable;
        ebase::event_emitter_waitor	on_connected;

        eio::socket_address addr;

        addr.parse_ip("127.0.0.1");
        ASSERT_TRUE( listen_handle->listen(addr));
        on_acceable.set_event(listen_handle->on_readable);

        addr = listen_handle->get_local_address();

        ASSERT_TRUE( addr.size()!=0 );
        ASSERT_TRUE( addr.port()!=0 );

        on_connected.set_event(connect_handle->on_opened);

        ASSERT_TRUE( connect_handle->open(addr) );

        ASSERT_TRUE(on_acceable.wait_event());//block in android

        accept_handle = listen_handle->accept();
        ASSERT_TRUE( accept_handle.valid() );
        ASSERT_TRUE(on_connected.wait_event());

        //ebase::thread::sleep(100000);
        //continue...

        on_connected.clear();
        on_acceable.clear();

        listen_handle->close();
        connect_handle->close();
        accept_handle->close();

    }


     
}

TEST( socket,accept )
{
  	int		memcount = ebase::allocator::get_allocated_count();

    eio::get_network_platform_loop()->start();
    eio::get_network_poll_loop()->start();


    test_socket_accept(eio::socket_native::socket_api_poll);

	test_socket_accept(eio::socket_native::socket_api_platform);

    eio::get_network_poll_loop()->abort_all();
    eio::get_network_poll_loop()->join();

    eio::get_network_platform_loop()->abort_all();
    eio::get_network_platform_loop()->join();

	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}


TEST(socket,socket_udp_pair)
{
	int		memcount = ebase::allocator::get_allocated_count();

	int out[2]={0};

	ASSERT_EQ(0,eio::socket_native::socket_udp_pair(out));
    ASSERT_NE(out[0], 0);
    ASSERT_NE(out[1], 0);
	eio::socket_native::close_udp_pair(out);


	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}
