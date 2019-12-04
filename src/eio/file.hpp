#pragma once

#include "../ebase/string.hpp"
#include "../ebase/buffer.hpp"
#include "../ebase/executor.hpp"
#include "../ebase/event_emitter.hpp"
#include "base/io_callback.hpp"
#include "base/io_request.hpp"
#include <stdio.h>
#ifdef _WIN32
int ftruncate64(int fd, int64_t offset);
#endif
namespace eio
{
//ReadDirectoryChangesW 可读取变化,MoveFileWithProgress,SetFilePointerEx
//linux,inotify api
//linux 下可以使用flock 文件锁来实现flags_exclusive https://www.ibm.com/developerworks/cn/linux/l-cn-filelock/
//win32 下有_locking
	class file;
	typedef ebase::ref_ptr<file>	file_ptr;

	class file : public ebase::executor_single_thread
	{
	public:
		file(ebase::executor* event_executor=0);
		~file();

        enum
        {
            file_api_default=0,
            file_api_posix,
            file_api_platform,//default,only for win32,when linux it use file_api_posix
        };
		static void set_defalt_api_type(int file_api_type=file_api_platform);

		static file_ptr	create_instance(ebase::executor* event_executor=0,int file_api_type=file_api_default);


		enum
		{
			flags_open_exists = 1,//error when file not exists
			flags_readonly=2,//readonly for io
			flags_truncate=4,//打开的时候,设置文件长度为0
			flags_exclusive = 8,//其它程序只能以flags_readonly模式打开
			flags_sequential = 16,//只有win32有实现
			flags_random = 32,//只有win32有实现
			flags_delete_on_close = 64,//只有win32有实现
		};

		enum
		{
			flags_pos_begin=0,
			flags_pos_current=1,
			flags_pos_end=2,
		};

		class request:public io_request
		{
		public:
			request( file*	_file_ptr );

			enum
			{
				operate_none=0,
				operate_write,
				operate_read,
			}					operate;

			ebase::buffer		io_data;
			int					io_size;
			int64_t				offset;//offset = -1写入文件末尾
			file_ptr			file_handle;

			file*				get_file();//此函数在io_request::callback调用后失效
		private:



			virtual void		internal_do_request();
		};

        virtual bool					is_opened();
		static file_ptr					open(const ebase::string& path,int flags,ebase::executor* event_executor);
		//in win32,
		//In the ANSI version of this function, the name is limited to MAX_PATH characters,
		//To extend this limit to 32,767 wide characters, call the function file_system::set_utf8_path_for_win32(true) and prepend "\\?\" to the path
		virtual bool					open(const ebase::string& path,int flags=0);
		virtual void					close();

		//在异步操作的时候，文件位置没有什么用,需要使用者自己计算io offset
        virtual int						eof();//1:eof,0:not eof,-1:error
		virtual int64_t					size();//出错返回-1
		virtual bool					seek(int64_t offset,int flags_pos =flags_pos_begin);
		virtual int64_t					tell();
		virtual bool					truncate(int64_t offset=-1);
		virtual bool					flush();

		//write 参数offset为-1的时候，为文件末尾,不能为其它负数
		//read 的时候offset>=0[MUST]
		virtual bool					read( size_t read_size,int64_t offset,const io_callback& callback);
		virtual bool					write( const ebase::buffer& buffer,int64_t offset,const io_callback& callback);
		
		virtual ebase::ref_ptr<request>	create_request();


	private:
		ebase::executor*		_event_executor;
		FILE*					_file;

        virtual void					internal_do_request(request* req) ;//实现者需要实现这个同步函数，实现文件读写
	};


}