#include "file_win32.hpp"
#include "../file_system.hpp"
#include "../../ebase/platform.hpp"


#include <io.h>


namespace eio
{
	
	file_win32::file_win32(ebase::executor* event_executor):file(event_executor),_hfile(INVALID_HANDLE_VALUE),_last_offset(0),_is_eof_flags(false)
	{
		ebase::executor_single_thread::set_parent( iocp_win32::instance() );
	}

	file_win32::~file_win32()
	{
		close();
	}

	bool file_win32::open(const ebase::string& name,int flags)
	{

		DWORD access_flags = GENERIC_WRITE |GENERIC_READ;
		DWORD share_flags = FILE_SHARE_READ|FILE_SHARE_WRITE;
		DWORD creation_flags = OPEN_ALWAYS;
		DWORD attribute = FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED;//FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE

		if(flags&flags_readonly)access_flags = GENERIC_READ;

		if(flags&flags_open_exists)
		{
			if(flags&flags_truncate)
				creation_flags = TRUNCATE_EXISTING;
			else
				creation_flags = OPEN_EXISTING;
		}else
		{
			if(flags&flags_truncate)
				creation_flags = CREATE_ALWAYS;
			else
				creation_flags = OPEN_ALWAYS;
		}

		if( flags & flags_random )attribute |= FILE_FLAG_RANDOM_ACCESS;
		if( flags & flags_sequential )attribute |= FILE_FLAG_SEQUENTIAL_SCAN;

		if(flags&flags_delete_on_close)
		{
			attribute |= (FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE);
			access_flags |= DELETE;
			share_flags |= FILE_SHARE_DELETE;
		}

		if(flags&flags_exclusive)share_flags = FILE_SHARE_READ;
		
		_hfile = ::CreateFileA( name.c_str(),access_flags,share_flags,0,creation_flags,attribute,0);

		if(_hfile!=INVALID_HANDLE_VALUE)iocp_win32::instance()->assign_handle( _hfile,0 );

		return _hfile!=INVALID_HANDLE_VALUE;
	}

	void file_win32::close()
	{
		if(_hfile!=INVALID_HANDLE_VALUE)CloseHandle(_hfile);
		_hfile = INVALID_HANDLE_VALUE;
	}

	int64_t file_win32::size()
	{
		if(_hfile==INVALID_HANDLE_VALUE)return -1;

		LARGE_INTEGER out = {0};
		if(!::GetFileSizeEx( _hfile,&out ))return -1;

		return out.QuadPart;
	}

	bool file_win32::seek(int64_t offset,int flags_pos)
	{
		if(_hfile==INVALID_HANDLE_VALUE)return false;

		LARGE_INTEGER	current;

		current.QuadPart = offset;

		if( TRUE==::SetFilePointerEx(_hfile,current,&current,flags_pos ) )
		{
			_last_offset=current.QuadPart;
			return true;
		}
		return false;
	}
	int64_t file_win32::get_file_pointer_real()
	{

		if(_hfile==INVALID_HANDLE_VALUE)return false;

		
		LARGE_INTEGER	current;

		current.QuadPart = 0;

		if( TRUE==::SetFilePointerEx(_hfile,current,&current,FILE_CURRENT) )
		{
			return current.QuadPart;
		}
		return -1;
	}

	
	int64_t file_win32::tell()
	{
		if(_hfile==INVALID_HANDLE_VALUE)return false;

		
		int64_t result = get_file_pointer_real();
		if( result>=0 )
		{
			return _last_offset;
		}

		return -1;
	}

	bool file_win32::truncate(int64_t offset)
	{
		if(_hfile==INVALID_HANDLE_VALUE)return false;

        if(-1==offset)this->seek( offset,flags_pos_begin);
		return TRUE==::SetEndOfFile(_hfile);
	}

	bool file_win32::is_opened()
	{
		return (_hfile!=INVALID_HANDLE_VALUE);
	}


	int file_win32::eof()
	{
		return _is_eof_flags;
	}

	bool file_win32::flush()
	{
		if(!is_opened())return false;

		return TRUE==::FlushFileBuffers( _hfile );
	}

	void file_win32::internal_do_request(request* req)
	{

	}

	bool file_win32::do_request(file::request* req)
	{
		BOOL result;
		DWORD error_code = 0;
		request_win32* p = (request_win32*)req;
		LARGE_INTEGER	offset;

		offset.QuadPart = p->offset;

		LPOVERLAPPED ol = p->iocp_data.init(&file_win32::completed,this);

		ol->Offset = offset.LowPart;
		ol->OffsetHigh = offset.HighPart;

		p->add_ref();
		p->iocp_data._request_win32 = p;
		if(req->operate == request::operate_read )
		{
			LPVOID p=req->io_data.resize(req->io_size,false);
			int capacity = req->io_size;
			req->io_size = 0;
			result = ::ReadFile( _hfile,p,capacity,(LPDWORD)&req->io_size,ol );
			
		}else if(req->operate == request::operate_write )
		{
			result = ::WriteFile( _hfile,req->io_data.data(),req->io_data.size(),(LPDWORD)&req->io_size,ol );
		}else
		{
			p->iocp_data._request_win32 = 0;
			p->release();
			req->error.set_system_error(ERROR_INVALID_HANDLE);
			return false;
		}
		error_code = ::GetLastError();

        this->_is_eof_flags = (!result && ERROR_HANDLE_EOF!=error_code);

		if(!result && ERROR_IO_PENDING!=error_code)
		{
			p->error.set_system_error(error_code);
			p->iocp_data._request_win32 = 0;
			p->release();
			return false;
		}

		return true;
	}

	ebase::ref_ptr<file::request> file_win32::create_request()
	{
		return new request_win32(this);
	}

	void file_win32::completed(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params)
	{
		request_win32::_iocp_data_* data=(request_win32::_iocp_data_*)ol_params;
		ebase::ref_ptr<request_win32>	prequest_win32 = (request_win32*)data->_request_win32;

		int error_code = prequest_win32->iocp_data.Internal;

		prequest_win32->io_size = complete_bytes;
		if(prequest_win32->operate == file::request::operate_read)prequest_win32->io_data.resize(complete_bytes,true);

		if(0==error_code)
		{
			if(-1==prequest_win32->offset)
			{
				_last_offset = this->size();
			}else
			{
				_last_offset = prequest_win32->offset+prequest_win32->io_size;
			}
		}

		prequest_win32->error.set_system_error(error_code);
		prequest_win32->do_completed();

		data->_request_win32 = 0;
		prequest_win32->release();
	}

	file_win32::request_win32::request_win32(file*	_file_ptr):file::request(_file_ptr)
	{
		
	}

	bool file_win32::request_win32::do_request()
	{

		file_win32* p = (file_win32*)this->file_handle.get();

		return p->do_request(this);
	}

};