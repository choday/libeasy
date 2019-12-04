#include "file_platform.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
int ftruncate64(int fd, int64_t offset)
{
    HANDLE hfile = (HANDLE)_get_osfhandle(fd);
    if (INVALID_HANDLE_VALUE == hfile)return -1;

    _lseeki64(fd, offset, SEEK_SET);
    int result = (TRUE == ::SetEndOfFile(hfile)) ? 0 : -1;
    if (-1 == result)errno = GetLastError();

    return result;
}

char *_real_path(const char *path, char *resolved_path)
{
    char* p = _fullpath( resolved_path,path,PATH_MAX );

    if(p && 0==_access( p,0 ) )return p;
    ::free(p);

    return 0;
}

char *_full_path(const char *path)
{
    int count = ::GetFullPathNameA( path,0,0,0 );
	if(0==count)return 0;

    char* p = (char*)::malloc(count+1);
    char* filename = 0;

	count = ::GetFullPathNameA( path,count,p,&filename );
	p[count]=0;

    if(count>0 && p[count-1] == '\\' )p[count-1]=0;

    return p;
}

char *_canonicalize_file_name(const char *path)
{
    char* p = ::_full_path(path);
  
    if(p && 0==_access( p,0 ) )return p;
    ::free(p);

    return 0;
}

char *_current_path()
{
	DWORD count = ::GetCurrentDirectoryA(0,0);
	if(!count)return 0;

    char* p = (char*)::malloc(count+1);
    count = ::GetCurrentDirectoryA(count,p);

    p[count]=0;
    if(count>0 && p[count-1] == '\\' )p[count-1]=0;
    return p;
}
char* _exe_path()
{

    char* p=(char*)malloc(PATH_MAX+1);
    int count = GetModuleFileNameA( 0,p,PATH_MAX );
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
int64_t     _file_size(const char *path)
{
    struct __stat64 data;
    if(-1==::_stat64(path,&data))return -1;
    return data.st_size;
}

uint16_t    _stat_mode(const char* path)
{
    struct _stat data; 
    if(-1==::_stat(path,&data))return 0;
    return data.st_mode;
}