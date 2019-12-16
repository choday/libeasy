#include<iostream>
#include<gtest/gtest.h>
#include <ebase/thread_loop.hpp>
#include <ebase/event_emitter.hpp>
#include <ehttp/http_server.hpp>
#include <ehttp/http_socket.hpp>
#include <eio/eio.hpp>

TEST(http_connection,open)
{    
    eio::get_network_platform_loop()->start();
    eio::get_network_poll_loop()->start();
ebase::thread_loop  loop;

    eio::socket_native::set_default_api_type( eio::socket_native::socket_api_poll );

    ehttp::http_socket_ptr connection=new ehttp::http_socket();

    connection->request.accept_encoding="";
    connection->request.url.parse("https://cloud.tencent.com/developer/section/1189886");
    //connection->request.url.parse("https://www.baidu.com");


connection->set_event_executor(&loop);

    class test:public ebase::ref_class<>
    {
    public:
        ehttp::http_socket_ptr connection;
        virtual void			notify_readable(ebase::ref_class_i* fire_from_handle)
        {
            ebase::buffer data;

            int total = 0;
            while(connection->read_buffer(data)>0)
            {
                ebase::string temp;
                total+=data.size();
                temp.assign((char*)data.data(),data.size());
                printf("%s",temp.c_str() );

                data.resize(0);
            }
        };
    } test_object;

    test_object.connection=connection;
    connection->open();

    connection->on_readable.set_function(&test::notify_readable,&test_object);

    loop.run();

	ebase::event_emitter_waitor	on_connected;

	on_connected.set_event( connection->on_readable );

    on_connected.wait_event(-1);

    ebase::buffer data;

    int total = 0;

    while(connection->read_buffer(data)>0)
    {
        ebase::string temp;
        total+=data.size();
        temp.assign((char*)data.data(),data.size());
        printf("%s",temp.c_str() );

        data.resize(0);
    }
    printf("\ntotal:%d\n",total);
connection->read_buffer(data);




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