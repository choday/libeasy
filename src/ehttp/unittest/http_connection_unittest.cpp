#include<iostream>
#include<gtest/gtest.h>
#include <ebase/thread_loop.hpp>
#include <ebase/event_emitter.hpp>
#include <ehttp/http_server.hpp>
#include <ehttp/http_connection.hpp>
#include <eio/eio.hpp>

TEST(http_connection,open)
{    
    eio::get_network_platform_loop()->start();
    eio::get_network_poll_loop()->start();

    eio::socket_native::set_default_api_type( eio::socket_native::socket_api_poll );

    ehttp::http_connection_ptr connection=new ehttp::http_connection();

    connection->open_request("GET","https://cloud.tencent.com/developer/section/1189886" );

	ebase::event_emitter_waitor	on_connected;

	on_connected.set_event( connection->on_opened );

    on_connected.wait_event(-1);

    ebase::buffer data;

    while(connection->read_buffer(data))
    {
        ebase::string temp;
        temp.assign((char*)data.data(),data.size());
        printf("recv\n%s\n",temp.c_str() );

        data.resize(0);
    }
ebase::thread_loop  loop;



    loop.run();

};

TEST(http_server,listen)
{
   return;
    ebase::thread_loop  loop;

    eio::socket_native::set_default_api_type( eio::socket_native::socket_api_poll );

    ehttp::http_server server;
    eio::get_network_platform_loop()->start();
    eio::get_network_poll_loop()->start();

    ASSERT_TRUE( server.listen(90) );


    loop.run();
};