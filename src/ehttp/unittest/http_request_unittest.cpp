#include<iostream>
#include<gtest/gtest.h>
#include <ebase/thread_loop.hpp>
#include <ehttp/http_request.hpp>
#include <ehttp/http_response.hpp>

TEST(http_url,to_string)
{

    ehttp::http_url request;

    ASSERT_TRUE( request.parse("https://u:p@www.baidu.com/path1/h.html?query=aaa&query2=b#c=xxx") );

    EXPECT_STRCASEEQ(request.schema.c_str(),"https");
    EXPECT_STRCASEEQ(request.host.c_str(),"www.baidu.com");
    EXPECT_STRCASEEQ(request.port.c_str(),"");
    EXPECT_STRCASEEQ(request.path.c_str(),"/path1/h.html");
    EXPECT_STRCASEEQ(request.query.c_str(),"query=aaa&query2=b");
    EXPECT_STRCASEEQ(request.fragment.c_str(),"c=xxx");
    EXPECT_STRCASEEQ(request.userinfo.c_str(),"u:p");
    EXPECT_STRCASEEQ(request.to_string(true).c_str(),"https://u:p@www.baidu.com/path1/h.html?query=aaa&query2=b#c=xxx");
    EXPECT_STRCASEEQ(request.to_string(false).c_str(),"https://u:p@www.baidu.com/path1/h.html?query=aaa&query2=b");

    ASSERT_FALSE( request.parse("u:p@www.baidu.com:99/path1/h.html?query=aaa&query2=b#c=xxx") );
    ASSERT_TRUE( request.parse("https://u:p@www.baidu.com:99/path1/h.html?query=aaa&query2=b#c=xxx") );

    EXPECT_STRCASEEQ(request.schema.c_str(),"https");
    EXPECT_STRCASEEQ(request.host.c_str(),"www.baidu.com");
    EXPECT_STRCASEEQ(request.port.c_str(),"99");
    EXPECT_STRCASEEQ(request.path.c_str(),"/path1/h.html");
    EXPECT_STRCASEEQ(request.query.c_str(),"query=aaa&query2=b");
    EXPECT_STRCASEEQ(request.fragment.c_str(),"c=xxx");
    EXPECT_STRCASEEQ(request.userinfo.c_str(),"u:p");


    ASSERT_TRUE( request.parse("https://www.baidu.com") );

    EXPECT_STRCASEEQ(request.schema.c_str(),"https");
    EXPECT_STRCASEEQ(request.host.c_str(),"www.baidu.com");
    EXPECT_STRCASEEQ(request.port.c_str(),"");
    EXPECT_STRCASEEQ(request.path.c_str(),"/");
    EXPECT_STRCASEEQ(request.query.c_str(),"");
    EXPECT_STRCASEEQ(request.fragment.c_str(),"");
    EXPECT_STRCASEEQ(request.userinfo.c_str(),"");
    EXPECT_STRCASEEQ(request.to_string(false).c_str(),"https://www.baidu.com/");

}

TEST(http_request,make_request)
{
    ehttp::http_request request;

    ASSERT_TRUE( request.url.parse("https://u:p@www.baidu.com/path1/h.html?query=aaa&query2=b#c=xxx") );
    request.keep_alive=true;
    request.chunked=false;

    ebase::buffer data = request.make_buffer();
    data.append("\0",1);

    char* p =
"GET /path1/h.html HTTP/1.1\r\n"
"Host: www.baidu.com\r\n"
"Accept-Encoding: gzip, deflate\r\n"
"Connection: keep-alive\r\n"
"\r\n";

    ASSERT_STRCASEEQ( (const char*)data.data(),p);

    //printf("%s\n",data.data() );

};



TEST(http_response,make_request)
{
    ehttp::http_response request;

    request.server = "elib";
    request.keep_alive=true;
    request.chunked=false;

    ebase::buffer data = request.make_buffer();
    data.append("\0",1);


    char* p =
"HTTP/1.1 200 OK\r\n"
"Connection: keep-alive\r\n"
"Content-Length: 0\r\n"
"Server: elib\r\n"
"\r\n";

    ASSERT_STRCASEEQ( (const char*)data.data(),p);

    

};