#pragma once
#include "../ebase/string.hpp"
#include "../ebase/precompile.h"

#ifdef _WIN32
#define dir_seperator '\\'
#else
#define dir_seperator '/'
#endif
namespace eio
{
//�ļ�ʱ��,linux stat(), ����utc timestamp,win32 ���ص�ʱ��since January 1, 1601 (UTC).
	class file_system
	{
	public:
        static bool		        file_exists(const char* path);
        static int64_t	        file_size(const char* path);//return -1 when fail
		static bool		        is_directory(const char* path);

		static ebase::string    real_path(const char* path);
        static ebase::string	full_path(const char* path);//����ȫ·��,������ļ��Ƿ����,
		static ebase::string	current_path();
		static ebase::string	exe_path();

		static bool		        mkdir(const char* path);//ֻ�ܴ�������Ŀ¼
        static bool		        mkfile(const char* path,const ebase::string& text=ebase::string());//����һ���ļ�,д���������ݣ�ע�⣬�˺���io������������ʹ��

		static bool		        rmdir(const char* path);//ֻ��ɾ����Ŀ¼
		static bool		        rmfile(const char* path);//linux unlink
		static bool		        rename(const char* old_name,const char* new_name);
	};

}