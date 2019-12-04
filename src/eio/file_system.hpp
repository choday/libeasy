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
//文件时间,linux stat(), 返回utc timestamp,win32 返回的时间since January 1, 1601 (UTC).
	class file_system
	{
	public:
        static bool		        file_exists(const char* path);
        static int64_t	        file_size(const char* path);//return -1 when fail
		static bool		        is_directory(const char* path);

		static ebase::string    real_path(const char* path);
        static ebase::string	full_path(const char* path);//计算全路径,不检测文件是否存在,
		static ebase::string	current_path();
		static ebase::string	exe_path();

		static bool		        mkdir(const char* path);//只能创建最终目录
        static bool		        mkfile(const char* path,const ebase::string& text=ebase::string());//创建一个文件,写入少量内容，注意，此函数io阻塞，请慎重使用

		static bool		        rmdir(const char* path);//只能删除空目录
		static bool		        rmfile(const char* path);//linux unlink
		static bool		        rename(const char* old_name,const char* new_name);
	};

}