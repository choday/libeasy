#include<iostream>
#include<gtest/gtest.h>

#include <ebase/atomic.hpp>


TEST(atomic,atomic)
{
	long value = 0;

	EXPECT_EQ(ebase::atomic::increment(&value),1);
	EXPECT_EQ(value,1);

	EXPECT_EQ(ebase::atomic::decrement(&value),0);
	EXPECT_EQ(value,0);


	EXPECT_EQ(ebase::atomic::fetch_and_or(&value,1),0);
	EXPECT_EQ(value,1);

	EXPECT_EQ(ebase::atomic::fetch_and_and(&value,1),1);
	EXPECT_EQ(value,1);

	value = 0;

	EXPECT_EQ(ebase::atomic::fetch_and_xor(&value,1),0);
	EXPECT_EQ(value,1);

	value = 0;
	EXPECT_EQ(ebase::atomic::fetch_and_add(&value,1),0);
	EXPECT_EQ(value,1);

	EXPECT_EQ(ebase::atomic::fetch_and_sub(&value,1),1);
	EXPECT_EQ(value,0);

    void* p =0;
    void* p2=&p;

    EXPECT_EQ((void*)0,ebase::atomic::set_pointer(&p,p2 ) );
    EXPECT_EQ(p,(void*)&p );

    EXPECT_EQ(p2,ebase::atomic::compare_and_set_pointer( &p,0,&p2 ));
    p=0;
    EXPECT_EQ(0,ebase::atomic::compare_and_set_pointer( &p,0,&p2 ));
    EXPECT_EQ(p,(void*)&p2 );
}

TEST(atomic,atomic_flags)
{

	ebase::atomic_flags	flags;

	EXPECT_EQ( flags.set( 0x11 ),0 );
	EXPECT_EQ( flags.value,0x11 );

	EXPECT_EQ( flags.clear(0x10),0x11 );
	EXPECT_EQ( flags.value,0x01 );

	EXPECT_TRUE( flags.test( 0x01 ) );

	EXPECT_TRUE( flags.test_bit_and_set(0) );
	EXPECT_TRUE( flags.clear_bit(0) );
	EXPECT_EQ( flags.value,0x0 );

}