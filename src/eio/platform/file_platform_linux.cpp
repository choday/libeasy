#include "file_platform.hpp"

char *_full_path(const char *path)
{

    return 0;
}


int64_t _file_size(const char *path)
{
#if _FILE_OFFSET_BITS==64
		struct stat64 result;

		if(0!=::stat64( path,&result ))return -1;

		return result.st_size;
#else
		struct stat result;

		if(0!=::stat( path,&result ))return -1;

		return result.st_size;
#endif
}

uint16_t       _stat_mode(const char* path)
{  
    struct stat data;    
    if(-1==::stat(path,&data))return 0;
    return data.st_mode;
}


char* _exe_path()
{

    char* p=(char*)malloc(PATH_MAX+1);
    int count = readlink("/proc/self/exe", p, PATH_MAX);
    if(count>0)
    {
        p[count] = 0;
    }else
    {
        ::free(p);
        return 0;
    }
       
    return p;
}