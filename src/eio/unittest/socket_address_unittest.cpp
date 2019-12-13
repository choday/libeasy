#include<iostream>
#include<gtest/gtest.h>
#include <eio/socket_address.hpp>

TEST(socket_address,parse_ipv6)
{

    eio::socket_address addr;

    const char* ipv6 = "1080:0:0:0:8:800:200C:417A";
    const unsigned short* pu=0;

    ASSERT_TRUE(addr.parse_ip(ipv6));

    pu = (const unsigned short*)addr.ip_data();

    ASSERT_EQ(pu[0],0x8010 );
    ASSERT_EQ(pu[1],0 );
    ASSERT_EQ(pu[2],0 );
    ASSERT_EQ(pu[3],0 );
    ASSERT_EQ(pu[4],0x0800 );
    ASSERT_EQ(pu[5],0x08 );
    ASSERT_EQ(pu[6],0x0c20 );
    ASSERT_EQ(pu[7],0x7a41 );

    
    ipv6 = "1080::8:800:200C:417A";
    ASSERT_TRUE(addr.parse_ip(ipv6));

    pu = (const unsigned short*)addr.ip_data();

    ASSERT_EQ(pu[0],0x8010 );
    ASSERT_EQ(pu[1],0 );
    ASSERT_EQ(pu[2],0 );
    ASSERT_EQ(pu[3],0 );
    ASSERT_EQ(pu[4],0x0800 );
    ASSERT_EQ(pu[5],0x08 );
    ASSERT_EQ(pu[6],0x0c20 );
    ASSERT_EQ(pu[7],0x7a41 );
    ASSERT_STRCASEEQ(addr.to_string(false).c_str(),ipv6);


    ipv6 = "FF01:0:0:0:0:0:0:101";
    ASSERT_TRUE(addr.parse_ip(ipv6));

    pu = (const unsigned short*)addr.ip_data();

    ASSERT_EQ(pu[0],0x01ff );
    ASSERT_EQ(pu[1],0 );
    ASSERT_EQ(pu[2],0 );
    ASSERT_EQ(pu[3],0 );
    ASSERT_EQ(pu[4],0 );
    ASSERT_EQ(pu[5],0 );
    ASSERT_EQ(pu[6],0 );
    ASSERT_EQ(pu[7],0x0101 );


    ipv6 = "FF01::101";
    ASSERT_TRUE(addr.parse_ip(ipv6));

    pu = (const unsigned short*)addr.ip_data();

    ASSERT_EQ(pu[0],0x01ff );
    ASSERT_EQ(pu[1],0 );
    ASSERT_EQ(pu[2],0 );
    ASSERT_EQ(pu[3],0 );
    ASSERT_EQ(pu[4],0 );
    ASSERT_EQ(pu[5],0 );
    ASSERT_EQ(pu[6],0 );
    ASSERT_EQ(pu[7],0x0101 );
    ASSERT_STRCASEEQ(addr.to_string(false).c_str(),ipv6);

    ipv6 = "0:0:0:0:0:0:0:1";
    ASSERT_TRUE(addr.parse_ip(ipv6));

    pu = (const unsigned short*)addr.ip_data();

    ASSERT_EQ(pu[0],0 );
    ASSERT_EQ(pu[1],0 );
    ASSERT_EQ(pu[2],0 );
    ASSERT_EQ(pu[3],0 );
    ASSERT_EQ(pu[4],0 );
    ASSERT_EQ(pu[5],0 );
    ASSERT_EQ(pu[6],0 );
    ASSERT_EQ(pu[7],0x100 );

    ipv6 = "::1";
    ASSERT_TRUE(addr.parse_ip(ipv6));

    pu = (const unsigned short*)addr.ip_data();

    ASSERT_EQ(pu[0],0 );
    ASSERT_EQ(pu[1],0 );
    ASSERT_EQ(pu[2],0 );
    ASSERT_EQ(pu[3],0 );
    ASSERT_EQ(pu[4],0 );
    ASSERT_EQ(pu[5],0 );
    ASSERT_EQ(pu[6],0 );
    ASSERT_EQ(pu[7],0x100 );
    ASSERT_STRCASEEQ(addr.to_string(false).c_str(),ipv6);

    ipv6 = "::ffff:202.120.2.30";
    ASSERT_TRUE(addr.parse_ip(ipv6));
    pu = (const unsigned short*)addr.ip_data();

    ASSERT_EQ(pu[0],0 );
    ASSERT_EQ(pu[1],0 );
    ASSERT_EQ(pu[2],0 );
    ASSERT_EQ(pu[3],0 );
    ASSERT_EQ(pu[4],0 );
    ASSERT_EQ(pu[5],0xffff );
    const unsigned char* pv4 = (const unsigned char*)&pu[6];

    ASSERT_EQ( pv4[0],202 );
    ASSERT_EQ( pv4[1],120 );
    ASSERT_EQ( pv4[2],2 );
    ASSERT_EQ( pv4[3],30 );
    ASSERT_STRCASEEQ(addr.to_string(false).c_str(),ipv6);

    
}
