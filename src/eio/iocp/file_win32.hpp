#pragma once
#include "../file.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "iocp_win32.hpp"


namespace eio
{

	class file_win32:public file
	{
	public:
		file_win32(ebase::executor* event_executor);
		~file_win32();

		virtual bool					open(const ebase::string& name,int flags) override;
		virtual void					close() override;

		virtual int64_t					size() override;
		virtual bool					seek(int64_t offset,int flags_pos =0) override;
		virtual int64_t					tell() override;
		virtual bool					truncate(int64_t offset=-1) override;
		virtual bool					is_opened() override ;
		virtual int						eof() override;
		virtual bool					flush() override;

		virtual void					internal_do_request(request* req) override;
		
        bool					do_request(request* req);

		virtual ebase::ref_ptr<file::request>	create_request() override;
		class request_win32:public file::request
		{
		public:
			request_win32(file*	_file_ptr);

			struct _iocp_data_:public iocp_win32::params	
			{
				ebase::ref_ptr<request_win32>	_request_win32;
			}	iocp_data;

			virtual bool		do_request();
		};
	private:
		HANDLE		_hfile;
        bool        _is_eof_flags;
		int64_t		_last_offset;
		int64_t		get_file_pointer_real();
		void		completed(void* io_key,unsigned long complete_bytes,struct iocp_win32::params* ol_params);
	};

};