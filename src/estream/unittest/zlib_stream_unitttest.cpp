#include<iostream>
#include<gtest/gtest.h>
#include <estream/zlib_stream.hpp>

TEST(zlib_stream,all)
{
    estream::zlib_stream zlib;

    zlib.disable_auto_release();
    zlib.init(true);

    ebase::buffer data;
    char* p = (char*)data.resize(1000);
    for(int i = 0;i<1000;++i)
    {
        *(p+i)=(char)rand()%20;
    }
    ASSERT_TRUE(zlib.write(data));
    ASSERT_TRUE(zlib.end());

    estream::zlib_stream zlib2;
    zlib2.init(false);
    zlib2.disable_auto_release();
    while( zlib.read(data) )
    {
        zlib2.write(data);
    }

    zlib2.end();

    ASSERT_EQ(zlib2.get_nread_size(),1000);

}