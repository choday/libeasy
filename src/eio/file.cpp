#define _CRT_SECURE_NO_WARNINGS

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "file_system.hpp"
#include "file.hpp"
#include "eio.hpp"
#include "../ebase/platform.hpp"
#include "platform/file_platform.hpp"

#ifdef _WIN32
#include "./iocp/file_win32.hpp"
#endif

//https://www.cnblogs.com/LittleHann/p/4582782.html

namespace eio
{
    int     global_default_file_api_type = file::file_api_platform;

	
    void file::set_defalt_api_type(int file_api_type/*=file_api_platform*/)
    {
        global_default_file_api_type = file_api_type;
    }

    eio::file_ptr file::create_instance(ebase::executor* event_executor,int file_api_type)
	{
        if(file_api_type==file_api_default)file_api_type=global_default_file_api_type;

        if( file_api_type == file_api_platform)
        {
#ifdef _WIN32
		return new file_win32(event_executor);
#else
		return new file(event_executor);
#endif
        }else //if( file_api_type == file_api_posix)
        {
            return new file(event_executor);
        }

	}


	file::file(ebase::executor* event_executor/*=0*/):_event_executor(event_executor),_file(0)
	{
		ebase::executor_single_thread::set_parent( get_fileio_executor() );
	}

	file::~file()
	{
		close();
	}

	bool file::is_opened()
	{
		return (0!=_file);
	}

	int file::eof()
	{
		if(!is_opened())return -1;

		return feof(_file);
	}

	bool file::flush()
	{
		if(!is_opened())return false;
		return 0==fflush(_file);
	}

	bool file::open(const ebase::string& path,int flags/*=0*/)
	{
		//_O_RANDOM _O_SEQUENTIAL
		int mode = O_BINARY| O_RDONLY;
		int pshared =SH_DENYNO;
		int pmode=0;

		if(0==(flags&flags_readonly))mode |= O_RDWR;
		if(0==(flags&flags_open_exists)){ mode |= O_CREAT;pmode=S_IWRITE|S_IREAD;}
		if( flags & flags_truncate )mode |= O_TRUNC;
		if( flags & flags_random )mode |= O_RANDOM;
		if( flags & flags_sequential )mode |= O_SEQUENTIAL;
		if( flags & flags_delete_on_close )mode |= O_TEMPORARY;

		if( flags & flags_exclusive )pshared = SH_DENYWR;

		//_creat,creatnew,FA_HIDDEN

        int fd = -1;
#ifdef _WIN32
		fd = ::_sopen(path.c_str(),mode,pshared,pmode);
#else
        fd = ::open(path.c_str(),mode, S_IRUSR| S_IWUSR| S_IRGRP| S_IWGRP| S_IROTH| S_IWOTH);
#endif
        if (-1 == fd)
        {
            //printf("open file fail:%s %d,%s,mode=%d\n", path.c_str(), errno, strerror(errno), mode);
            return false;
        }
        if (flags & flags_open_exists)
        {
            _file = ::fdopen(fd, "rb");
        }
        else
        {
            _file = ::fdopen(fd, "r+b");
        }
        
		return _file!=0;
	}

	eio::file_ptr file::open(const ebase::string& path,int flags,ebase::executor* event_executor)
	{
		file_ptr pf = file::create_instance( event_executor );

		if(!(pf->open(path,flags)))pf=0;

		return pf;
	}

	void file::close()
	{
		if(0!=_file)
		{
			int result = ::fclose(_file);
            if (result)
            {
                //printf("close file fail: %d,%s\n",  errno, strerror(errno));
            }
		}
		_file= 0;
	}

	int64_t file::size()
	{
        int64_t pos = ::ftelli64(_file);

		if(!this->seek(0,flags_pos_end))return -1;
        int64_t result = ::ftelli64(_file);
		
		this->seek(pos,flags_pos_begin);
		return result;
	}

	bool file::seek(int64_t offset,int flags_pos /*=flags_pos_begin*/)
	{
		if(!is_opened())return false;
        return -1!=::fseeki64(_file,offset,flags_pos);
	}

	int64_t file::tell()
	{
		if(!is_opened())return -1;
        int64_t pos = ::ftelli64(_file);
		return pos;
	}

    bool file::truncate(int64_t offset/*=-1*/)
    {
		if(!is_opened())return false;
		if(-1==offset) offset = ::ftelli64(_file);
        return 0==ftruncate64(::fileno(_file),offset );
    }

	bool file::read(size_t read_size,int64_t offset,const io_callback& callback)
	{	
		
		if(offset<0)return false;
		if(!is_opened())return false;
		ebase::ref_ptr<request> ptr = this->create_request();

		ptr->operate = request::operate_read;
		ptr->offset = offset;

		ptr->io_size = read_size;

		ptr->completed_callback = callback;
		ptr->completed_executor = this->_event_executor;

		return ptr->start();
	}

	bool file::write(const ebase::buffer& buffer,int64_t offset,const io_callback& callback)
	{
		if(offset<0 && -1!=offset)return false;
		if(!is_opened())return false;
		ebase::ref_ptr<request> ptr = this->create_request();

		ptr->operate = request::operate_write;
		ptr->offset = offset;

		ptr->io_data = buffer;
		ptr->io_size = buffer.size();

		ptr->completed_callback = callback;
		ptr->completed_executor = this->_event_executor;

		return ptr->start();
	}

	void file::internal_do_request(request* req)
	{
		int result=0;
		int64_t offset = req->offset;

		if(-1==offset)
		{
            offset = ::fseeki64(_file,0,SEEK_END);
		}else
		{
            offset = ::fseeki64(_file,offset,SEEK_SET );
		}

		if( -1== offset )
		{
			req->error.code = errno;
			req->error.message = strerror(errno);
			return ;
		}


		if(request::operate_write == req->operate)
		{
			result = ::fwrite( req->io_data.data(),1,req->io_data.size(),_file );

		}else if(request::operate_read == req->operate )
		{
			void* p=req->io_data.alloc( req->io_size );

			result = ::fread( p,1,req->io_size,_file );
			if(result)req->io_data.resize(result,true);

		}else
		{
			req->error.set_system_error(-1);
			return;
		}

		if(0==result)
		{
			req->error.code = errno;
			req->error.message = strerror(errno);
		}else
		{
			req->io_size = result;
		}
	}

	ebase::ref_ptr<file::request> file::create_request()
	{
		return new request(this);
	}

	file::request::request(file* _file_ptr):io_size(0),offset(0),file_handle(_file_ptr)
	{
		_request_executor = file_handle;
	}

	file* file::request::get_file()
	{
		return file_handle.get();;
	}

	void file::request::internal_do_request()
	{
		file_handle->internal_do_request(this);
	}
};