#include<iostream>
#include<gtest/gtest.h>
#include <eio/file.hpp>
#include <eio/file_system.hpp>
#include <ebase/thread_loop.hpp>
#include <ebase/allocator.hpp>

void test_real_path_AND_current_dir()
{

	ebase::string current_dir=eio::file_system::current_path();

	ASSERT_GT( current_dir.length() , 0 );
	ASSERT_NE( current_dir.data()[current_dir.length()-1],dir_seperator );
	ASSERT_TRUE(eio::file_system::is_directory(current_dir));

	ebase::string full_path1=eio::file_system::real_path("./");

	ASSERT_GT( full_path1.length() , 0 );
    ASSERT_NE(full_path1.data()[full_path1.length() - 1], dir_seperator);

	current_dir = eio::file_system::exe_path();
	ASSERT_GT( current_dir.length() , 0 );
}

TEST(file_system,full_path_AND_current_dir)
{
int memcount=ebase::allocator::get_allocated_count();

	test_real_path_AND_current_dir();

ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);

}

void test_mkdir_AND_rmdir()
{

	if(eio::file_system::file_exists("test_for_mkdir"))
	{
		eio::file_system::rmdir("test_for_mkdir");
	}

	ASSERT_FALSE( eio::file_system::file_exists("test_for_mkdir") );
	ASSERT_FALSE( eio::file_system::is_directory("test_for_mkdir") );
	ASSERT_EQ( eio::file_system::file_size("test_for_mkdir"),-1 );	

	ASSERT_TRUE( eio::file_system::mkdir("test_for_mkdir") );

	ASSERT_TRUE( eio::file_system::file_exists("test_for_mkdir") );
	ASSERT_TRUE( eio::file_system::is_directory("test_for_mkdir") );

	ASSERT_TRUE( eio::file_system::rename("test_for_mkdir","test_for_mkdir_new") );
	ASSERT_FALSE( eio::file_system::rmdir("test_for_mkdir") );

	ASSERT_FALSE( eio::file_system::file_exists("test_for_mkdir") );
	ASSERT_FALSE( eio::file_system::is_directory("test_for_mkdir") );

	ASSERT_TRUE( eio::file_system::file_exists("test_for_mkdir_new") );
	ASSERT_TRUE( eio::file_system::is_directory("test_for_mkdir_new") );

	ASSERT_TRUE( eio::file_system::rmdir("test_for_mkdir_new") );

}

TEST(file_system,mkdir_AND_rmdir)
{
	int memcount=ebase::allocator::get_allocated_count();

	test_mkdir_AND_rmdir();
ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}

void test_mkfile_AND_rmfile( )
{

	if(eio::file_system::file_exists("test.txt"))
	{
		eio::file_system::rmfile("test.txt");
	}

	ASSERT_FALSE( eio::file_system::file_exists("test.txt") );
	ASSERT_FALSE( eio::file_system::is_directory("test.txt") );
	ASSERT_EQ( eio::file_system::file_size("test.txt"),-1 );	

	ASSERT_TRUE( eio::file_system::mkfile("test.txt","test") );
    ASSERT_EQ( eio::file_system::file_size("test.txt"),4 );


	ASSERT_TRUE( eio::file_system::file_exists("test.txt") );
	ASSERT_FALSE( eio::file_system::is_directory("test.txt") );

	ASSERT_TRUE( eio::file_system::mkfile("test.txt","text") );
	ASSERT_EQ( eio::file_system::file_size("test.txt"),4 );

	ASSERT_TRUE( eio::file_system::rename("test.txt","new_test.txt") );
	ASSERT_FALSE( eio::file_system::rmfile("test.txt") );

	ASSERT_FALSE( eio::file_system::file_exists("test.txt") );
	ASSERT_FALSE( eio::file_system::is_directory("test.txt") );
	ASSERT_EQ( eio::file_system::file_size("test.txt"),-1 );	

	ASSERT_TRUE( eio::file_system::file_exists("new_test.txt") );
	ASSERT_FALSE( eio::file_system::is_directory("new_test.txt") );
	ASSERT_EQ( eio::file_system::file_size("new_test.txt"),4 );

	ASSERT_TRUE( eio::file_system::rmfile("new_test.txt") );
}

TEST(file_system,mkfile_AND_rmfile)
{
	int memcount=ebase::allocator::get_allocated_count();

	test_mkfile_AND_rmfile();
	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}