#define _CRT_SECURE_NO_WARNINGS
#include "file_system.hpp"
#include "platform/file_platform.hpp"

namespace eio
{

    ebase::string file_system::real_path(const char* path)
    {
        char* p = ::_canonicalize_file_name(path);
        ebase::string result(p);
        if(p)::free(p);

        return result;
    }

    ebase::string file_system::full_path(const char* path)
    {
        char* p = ::_full_path(path);
        ebase::string result(p);
        if(p)::free(p);

        return result;
    }

    bool file_system::file_exists(const char* path)
    {
        return 0==::_access(path,0);
    }

    //gcc -D_FILE_OFFSET_BITS=64
    int64_t file_system::file_size(const char* path)
    {
        return ::_file_size(path);
    }

    bool file_system::is_directory(const char* path)
    {
        return (::_stat_mode(path)&S_IFMT)==S_IFDIR;
    }

    ebase::string file_system::current_path()
    {
        char* p = ::_current_path();
        ebase::string result(p);
        if(p)::free(p);

        return result;
    }
    
    ebase::string file_system::exe_path()
    {

        char* p = ::_exe_path();
        ebase::string result(p);
        if(p)::free(p);

        return result;
    }

    bool file_system::mkdir(const char* path)
    {
        return 0==::_mkdir(path);
    }

    bool file_system::mkfile(const char* path,const ebase::string& text)
	{
		FILE* pf = fopen(path,"w+");
		if(0==pf)return false;
		if(text.length())fwrite( text.data(),text.length(),1,pf );
		fclose(pf);
		return true;
	}

    bool file_system::rmdir(const char* path)
    {
        return 0==::_rmdir(path);
    }

    bool file_system::rmfile(const char* path)
    {
        return 0==::_unlink(path);
    }

    bool file_system::rename(const char* old_name,const char* new_name)
    {
        return 0==::rename(old_name,new_name);
    }

}