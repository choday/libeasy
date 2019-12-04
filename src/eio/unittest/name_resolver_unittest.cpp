#include<iostream>
#include<gtest/gtest.h>
#include <eio/name_resolver.hpp>
#include <ebase/thread.hpp>

void test_name_resolver()
{
	
	eio::io_callback_waitor callback;
	ebase::ref_ptr<eio::name_resolver::request> request;

  
	ASSERT_TRUE( eio::name_resolver::resolver( "www.baidu.com","http",callback,0 ) );

	request = (eio::name_resolver::request*)callback.wait_event(-1);

	ASSERT_TRUE( request.valid() );
	ASSERT_EQ( request->error.code,0 );

	ASSERT_TRUE( request->is_success() );
	ASSERT_TRUE( request->current()!=0 );

	request=0;



	ASSERT_TRUE( eio::name_resolver::resolver( "127.0.0.1","80",callback,0 ) );

	request = (eio::name_resolver::request*)callback.wait_event(-1);

	ASSERT_TRUE( request.valid() );
	ASSERT_EQ( request->error.code,0 );

	ASSERT_TRUE( request->is_success() );
	ASSERT_TRUE( request->current()!=0 );

	request=0;


	ASSERT_TRUE( eio::name_resolver::resolver( "fe80::f9cf:cbbc:8be8:cb13%11","80",callback,0 ) );

	request = (eio::name_resolver::request*)callback.wait_event(5000);
    callback.clear();
	ASSERT_TRUE( request.valid() );
	ASSERT_EQ( request->error.code,0 );

	ASSERT_TRUE( request->is_success() );
	ASSERT_TRUE( request->current()!=0 );

	request=0;

    
}

TEST(test_name_resolver,resolver)
{
	int		memcount = ebase::allocator::get_allocated_count();
	test_name_resolver();

	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}