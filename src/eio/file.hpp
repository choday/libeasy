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
//ReadDirectoryChangesW �ɶ�ȡ�仯,MoveFileWithProgress,SetFilePointerEx
//linux,inotify api
//linux �¿���ʹ��flock �ļ�����ʵ��flags_exclusive https://www.ibm.com/developerworks/cn/linux/l-cn-filelock/
//win32 ����_locking
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
			flags_truncate=4,//�򿪵�ʱ��,�����ļ�����Ϊ0
			flags_exclusive = 8,//��������ֻ����flags_readonlyģʽ��
			flags_sequential = 16,//ֻ��win32��ʵ��
			flags_random = 32,//ֻ��win32��ʵ��
			flags_delete_on_close = 64,//ֻ��win32��ʵ��
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
			int64_t				offset;//offset = -1д���ļ�ĩβ
			file_ptr			file_handle;

			file*				get_file();//�˺�����io_request::callback���ú�ʧЧ
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

		//���첽������ʱ���ļ�λ��û��ʲô��,��Ҫʹ�����Լ�����io offset
        virtual int						eof();//1:eof,0:not eof,-1:error
		virtual int64_t					size();//������-1
		virtual bool					seek(int64_t offset,int flags_pos =flags_pos_begin);
		virtual int64_t					tell();
		virtual bool					truncate(int64_t offset=-1);
		virtual bool					flush();

		//write ����offsetΪ-1��ʱ��Ϊ�ļ�ĩβ,����Ϊ��������
		//read ��ʱ��offset>=0[MUST]
		virtual bool					read( size_t read_size,int64_t offset,const io_callback& callback);
		virtual bool					write( const ebase::buffer& buffer,int64_t offset,const io_callback& callback);
		
		virtual ebase::ref_ptr<request>	create_request();


	private:
		ebase::executor*		_event_executor;
		FILE*					_file;

        virtual void					internal_do_request(request* req) ;//ʵ������Ҫʵ�����ͬ��������ʵ���ļ���д
	};


}