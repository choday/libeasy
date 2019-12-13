#include<iostream>
#include<gtest/gtest.h>
#include <ebase/thread_loop.hpp>
#include <ehttp/http_protocol.hpp>
#include <eio/eio.hpp>

TEST(http_protocol,parser)
{
    const char* p =
"GET / HTTP/1.1\r\n"
"Upgrade: websocket\r\n"
"Host: 127.0.0.1:90\r\n"
"Connection: keep-alive\r\n"
"Cache-Control: max-age=0\r\n"
"Upgrade-Insecure-Requests: 1\r\n"
"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36\r\n"
"Sec-Fetch-User: ?1\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\n"
"Sec-Fetch-Site: cross-site\r\n"
"Sec-Fetch-Mode: navigate\r\n"
"Accept-Encoding: gzip, deflate, br\r\n"
"Accept-Language: zh-CN,zh;q=0.9\r\n"
"\r\n";
    class test:public ehttp::http_protocol 
    {
    public:
        test()
        {
            body_data_length=0;
        }
        size_t body_data_length;

        void reset()
        {
            ehttp::http_protocol::reset();

            body_data_length=0;
            
        }
        
    }parse;

    int len = (int)strlen(p);

    parse.parse( p,len );
    //printf("%d:%s(%s)\n",parse.get_error_code(),parse.get_error_name(),parse.get_error_message() );
//ASSERT_EQ(parse.body_data_length,0);
//ASSERT_STRCASEEQ( parse.method.c_str(),"GET" );
//parse.reset();

    for(int i=0;i<len;++i)
    {
        //parse.parse( p+i,1 );
    }
    //ASSERT_EQ(parse.body_data_length,0);
   // ASSERT_STRCASEEQ( parse.method.c_str(),"GET" );


    p=
"HTTP/1.1 200 OK\r\n"
"Cache-Control: private\r\n"
"Connection: Keep-Alive\r\n"
"Content-Encoding: gzip\r\n"
"Content-Type: text/html\r\n"
"Date: Sun, 08 Dec 2019 14:11:15 GMT\r\n"
"Expires: Sun, 08 Dec 2019 14:10:23 GMT\r\n"
"Server: BWS/1.1\r\n"
"Set-Cookie: delPer=0; path=/; domain=.baidu.com\r\n"
"Set-Cookie: BD_HOME=0; path=/\r\n"
"Set-Cookie: H_PS_PSSID=1467_21101_30210_30071_26350; path=/; domain=.baidu.com\r\n"
"Transfer-Encoding: chunked\r\n"
"\r\n"
"1000\r\n1\r\n"
"00\r\n";

len = (int)strlen(p);
    parse.parse( p,len );
    parse.parse( p,len );
};