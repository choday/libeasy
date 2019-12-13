#include<iostream>
#include<gtest/gtest.h>
#include <ebase/allocator.hpp>
#include <ebase/buffer.hpp>

TEST(buffer,resize)
{
int memcount=ebase::allocator::get_allocated_count();

    ebase::buffer test,test333;

    char p[]="123123123123123";
    test.assign(p,sizeof(p));

    test333=test;
    ASSERT_EQ(test333.resize(0),(void*)0);
    ASSERT_NE(test.resize(0),(void*)0);

    test.assign(p,sizeof(p));
    ASSERT_GE(test.size(),sizeof(p));
    ASSERT_GE(test.capacity(),sizeof(p));
    ASSERT_STRCASEEQ( (const char*)test.resize(sizeof(p),true),p );

    char p2[]="ddddddddddddffffddddddddddddffffffffffffffffffffffffffffffffffffffffffffffffffff";
    test.append(p2,sizeof(p2));
    ASSERT_GE(test.size(),sizeof(p)+sizeof(p2)-1);
    ASSERT_GE(test.capacity(),sizeof(p)+sizeof(p2)-1);

    ASSERT_NE(test.resize(0),(void*)0);

    ASSERT_EQ( test.size(),0 );
    ASSERT_GE(test.capacity(),sizeof(p)+sizeof(p2)-1);

    test.append(p2,sizeof(p2));
    ebase::buffer_list blist;

    blist.push_back(test);
    ASSERT_EQ(blist.count(),1);

    ebase::buffer front = blist.front();
    ASSERT_EQ(front.size(),test.size() );

    ASSERT_EQ(test.data(),front.data());
    ASSERT_FALSE( test.is_empty());

    test.assign(p,sizeof(p));

    ASSERT_NE(test.data(),front.data());

    front.alloc(100);
    ASSERT_EQ( front.size(),0 );
    ASSERT_TRUE( front.is_empty());
    ASSERT_GE(front.capacity(),100);

 
    test.clear();
    front.clear();
    blist.clear();

    ASSERT_EQ( test.size(),0 );
    ASSERT_EQ( front.size(),0 );
    ASSERT_EQ(blist.count(),0);

    ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}