#include<iostream>
#include<gtest/gtest.h>
#include <eio/file.hpp>
#include <eio/file_system.hpp>
#include <ebase/thread_loop.hpp>
#include <ebase/allocator.hpp>

TEST(file,eof)//这个需要专门测试
{

}

void test_file_flags_delete_on_close()
{

	const char* filename = "file_open_flags_delete_on_close.txt";

	ASSERT_FALSE( eio::file_system::file_exists(filename) );
	ASSERT_FALSE( eio::file_system::is_directory(filename) );
	ASSERT_EQ( eio::file_system::file_size(filename),-1 );	

	eio::file_ptr file = eio::file::open(filename,eio::file::flags_delete_on_close,0 );
		
	ASSERT_TRUE( eio::file_system::file_exists(filename) );
	ASSERT_EQ( eio::file_system::file_size(filename),0 );

	file->close();
	ASSERT_FALSE( eio::file_system::file_exists(filename) );
	ASSERT_EQ( eio::file_system::file_size(filename),-1 );	

	file = eio::file::open(filename,eio::file::flags_delete_on_close,0 );
		
	ASSERT_TRUE( eio::file_system::file_exists(filename) );
	ASSERT_EQ( eio::file_system::file_size(filename),0 );

	file=0;
	ASSERT_FALSE( eio::file_system::file_exists(filename) );
	ASSERT_EQ( eio::file_system::file_size(filename),-1 );	

}
TEST(file,flags_delete_on_close)
{

	int		memcount = ebase::allocator::get_allocated_count();
#ifdef _WIN32
	eio::file::set_defalt_api_type(eio::file::file_api_platform);
    test_file_flags_delete_on_close();
	eio::file::set_defalt_api_type(eio::file::file_api_posix);

	test_file_flags_delete_on_close();
#endif

	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}

void test_file_flags_truncate()
{


	const char* filename = "file_open_flags_truncate.txt";

	if(eio::file_system::file_exists(filename))
	{
		eio::file_system::rmfile(filename);
	}

	ASSERT_TRUE( eio::file_system::mkfile(filename,filename) );

	ASSERT_TRUE( eio::file_system::file_exists(filename) );
	ASSERT_FALSE( eio::file_system::is_directory(filename) );
	ASSERT_EQ( eio::file_system::file_size(filename),strlen(filename) );	

	eio::file_ptr file = eio::file::open(filename,eio::file::flags_truncate,0 );
		
	ASSERT_TRUE( eio::file_system::file_exists(filename) );
	ASSERT_EQ( eio::file_system::file_size(filename),0 );

	file = 0;
	
    ASSERT_TRUE( eio::file_system::mkfile(filename,filename) );
	ASSERT_TRUE( eio::file_system::file_exists(filename) );
    file = eio::file::open(filename,eio::file::flags_open_exists,0 );

    ASSERT_TRUE(file.valid());
    ASSERT_TRUE( file->truncate(0) );
	EXPECT_EQ( eio::file_system::file_size(filename),0 );
	
	eio::file_system::rmfile(filename);

}
TEST(file,flags_truncate)
{
	
	int		memcount = ebase::allocator::get_allocated_count();

    eio::file::set_defalt_api_type(eio::file::file_api_posix);
    test_file_flags_truncate();
    eio::file::set_defalt_api_type(eio::file::file_api_platform);
	test_file_flags_truncate();	

	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}

void test_file_flags_open_exists()
{

	const char* filename = "file_open_flags_open_exists.txt";

	if(eio::file_system::file_exists(filename))
	{
		eio::file_system::rmfile(filename);
	}

	ASSERT_FALSE( eio::file_system::file_exists(filename) );

	eio::file_ptr file = eio::file::create_instance(0);

	ASSERT_FALSE(file->open(filename,eio::file::flags_open_exists));


	ASSERT_FALSE( eio::file_system::file_exists(filename) );
	ASSERT_EQ( eio::file_system::file_size(filename),-1 );

	
	ASSERT_TRUE( eio::file_system::mkfile(filename,filename) );

	ASSERT_TRUE(file->open(filename,eio::file::flags_open_exists));
	ASSERT_EQ( eio::file_system::file_size(filename),strlen(filename) );

#ifdef _WIN32
	ASSERT_FALSE( eio::file_system::rmfile(filename) );
#endif
	file=0;
	ASSERT_TRUE( eio::file_system::rmfile(filename) );


}

TEST(file,flags_open_exists)
{

	int		memcount = ebase::allocator::get_allocated_count();

	eio::file::set_defalt_api_type(eio::file::file_api_platform);
    test_file_flags_open_exists();
	eio::file::set_defalt_api_type(eio::file::file_api_posix);
	test_file_flags_open_exists();


	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}


void test_file_open_flags_exclusive()
{

	const char* filename = "file_open_flags_exclusive.txt";

	if(eio::file_system::file_exists(filename))
	{
		eio::file_system::rmfile(filename);
	}

	ASSERT_FALSE( eio::file_system::file_exists(filename) );

	eio::file_ptr file = eio::file::create_instance(0);
	eio::file_ptr file2 = eio::file::create_instance(0);

	ASSERT_TRUE(file->open(filename,eio::file::flags_exclusive));
	ASSERT_TRUE(eio::file_system::file_exists(filename));
	ASSERT_FALSE(file2->open(filename,eio::file::flags_open_exists));
	ASSERT_TRUE(file2->open(filename,eio::file::flags_readonly));

	ASSERT_FALSE( eio::file_system::rmfile(filename) );

	file2->close();
	file2=0;
	file=0;

	ASSERT_TRUE( eio::file_system::rmfile(filename) );
}

TEST(file,flags_exclusive)
{
	int		memcount = ebase::allocator::get_allocated_count();
#ifdef _WIN32
	eio::file::set_defalt_api_type(eio::file::file_api_platform);
    test_file_open_flags_exclusive();
	eio::file::set_defalt_api_type(eio::file::file_api_posix);
	test_file_open_flags_exclusive();
#endif

	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}

void test_file_size()
{
	const char* filename = "test_file_size.txt";

	if(eio::file_system::file_exists(filename))
	{
		eio::file_system::rmfile(filename);
	}

	ASSERT_FALSE( eio::file_system::file_exists(filename) );

	ASSERT_TRUE( eio::file_system::mkfile(filename,filename) );

	eio::file_ptr file = eio::file::open(filename,eio::file::flags_open_exists,0 );

	ASSERT_TRUE(file.valid());

	ASSERT_EQ( file->tell(),0) ;
	ASSERT_EQ(file->size(),strlen(filename) );
	ASSERT_EQ( file->tell(),0) ;

	file = 0;

	eio::file_system::rmfile(filename);
}

TEST(file,size)
{
	int		memcount = ebase::allocator::get_allocated_count();
	eio::file::set_defalt_api_type(eio::file::file_api_platform);
    test_file_size();
	eio::file::set_defalt_api_type(eio::file::file_api_posix);
	test_file_size();

	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}


void test_file_pointer()
{
	const char* filename = "test_file_pointer.txt";

	if(eio::file_system::file_exists(filename))
	{
		eio::file_system::rmfile(filename);
	}

	ASSERT_FALSE( eio::file_system::file_exists(filename) );

	ASSERT_TRUE( eio::file_system::mkfile(filename,filename) );

	eio::file_ptr file = eio::file::open(filename,eio::file::flags_open_exists,0 );

	ASSERT_TRUE(file.valid());


	int file_size = (int)strlen(filename);
	ASSERT_EQ(file->size(),file_size );


	ASSERT_EQ( file->tell(),0) ;
	ASSERT_TRUE( file->seek( 1,eio::file::flags_pos_begin) );
	ASSERT_EQ( file->tell(),1) ;

	ASSERT_TRUE( file->seek( 1,eio::file::flags_pos_current) );
	ASSERT_EQ( file->tell(),2 ) ;

	ASSERT_TRUE( file->seek( -1,eio::file::flags_pos_current) );
	ASSERT_EQ( file->tell(),1 ) ;

    ASSERT_EQ(0,file->eof());

	ASSERT_TRUE( file->seek( 0,eio::file::flags_pos_end) );
	ASSERT_EQ( file->tell(),file_size ) ;


	ASSERT_FALSE( file->seek( -1,eio::file::flags_pos_begin) );
	ASSERT_EQ( file->tell(),file_size) ;


	ASSERT_TRUE( file->seek( 1,eio::file::flags_pos_end) );
	ASSERT_EQ( file->tell(),file_size+1 ) ;
	

	ASSERT_EQ(file->size(),file_size );

	ASSERT_TRUE(file->truncate());
	ASSERT_EQ(file->size(),file_size+1 );


	file = 0;

	eio::file_system::rmfile(filename);
}

TEST(file,file_pointer)
{
	int		memcount = ebase::allocator::get_allocated_count();
	eio::file::set_defalt_api_type(eio::file::file_api_platform);
    test_file_pointer();
	eio::file::set_defalt_api_type(eio::file::file_api_posix);
	test_file_pointer();


	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}


void test_file_read_AND_write()
{
	const char* filename = "test_file_read_AND_write.txt";

	if(eio::file_system::file_exists(filename))
	{
		ASSERT_TRUE(eio::file_system::rmfile(filename));
	}

	eio::file_ptr _file = eio::file::open(filename,0,0);
	eio::io_callback_waitor	callback;


	ebase::buffer buffer;

	buffer.assign("test",4);
 
	EXPECT_FALSE( _file->write(buffer,-3,callback) );
	ASSERT_TRUE( _file->write(buffer,-1,callback) );

	eio::file::request* p = (eio::file::request*)callback.wait_event(-1);
	ASSERT_TRUE(p!=0);

			ASSERT_EQ(0,p->error.code );
			ASSERT_EQ(4,p->io_size );
			ASSERT_EQ( _file->size(),_file->tell() );
			
	EXPECT_FALSE( _file->read(4,-1,callback ));

	ASSERT_TRUE( _file->read(4,0,callback ));

	p = (eio::file::request*)callback.wait_event(-1);
	ASSERT_TRUE(p!=0);

			ASSERT_EQ( 4,_file->tell() );

			ASSERT_EQ(0,p->error.code );
			ASSERT_EQ(4,p->io_size );
			ASSERT_TRUE( memcmp( p->io_data.data(),"test",4 )==0 );

	_file->close();
	_file = 0;

    callback.clear();

	ASSERT_EQ(eio::file_system::file_size(filename),4 );


	{
		_file = eio::file::open(filename,0,0);//13,Permission denied in android
		eio::io_callback_waitor	callback;

		ebase::buffer buffer;

		buffer.assign("test",4);

        ASSERT_TRUE(_file.valid());

		EXPECT_FALSE( _file->write(buffer,-3,callback) );

		ASSERT_TRUE( _file->write(buffer,-1,callback) );
;
		eio::file::request* p = (eio::file::request*)callback.wait_event(-1);
		ASSERT_TRUE(p!=0);

				ASSERT_EQ(0,p->error.code );
				ASSERT_EQ(4,p->io_size );
				ASSERT_EQ( _file->size(),_file->tell() );
				
		EXPECT_FALSE( _file->read(4,-1,callback ));

		ASSERT_TRUE( _file->read(4,0,callback ));

		p = (eio::file::request*)callback.wait_event(-1);
		ASSERT_TRUE(p!=0);

				ASSERT_EQ( 4,_file->tell() );

				ASSERT_EQ(0,p->error.code );
				ASSERT_EQ(4,p->io_size );
				ASSERT_TRUE( memcmp( p->io_data.data(),"test",4 )==0 );
 
		_file->close();
		_file = 0;
        callback.clear();

	}

	ASSERT_EQ(eio::file_system::file_size(filename),8 );
	ASSERT_TRUE(eio::file_system::rmfile(filename));
}

TEST(file,read_AND_write)
{
	int		memcount = ebase::allocator::get_allocated_count();


	eio::file::set_defalt_api_type(eio::file::file_api_posix);
	test_file_read_AND_write();
    eio::file::set_defalt_api_type(eio::file::file_api_platform);
    test_file_read_AND_write();

	ASSERT_EQ(ebase::allocator::get_allocated_count(),memcount);
}