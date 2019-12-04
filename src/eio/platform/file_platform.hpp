#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#include  <io.h>
#include <direct.h>
#include <share.h>


int         ftruncate64(int fd, int64_t offset);
char *      _canonicalize_file_name(const char *path);//need user call free()
char *      _real_path(const char *path, char *resolved_path);
char *      _full_path(const char *path);//need user call free()
char *      _current_path();//need user call free(),,结尾的斜杠已去掉
char*       _exe_path();//need user call free()



int64_t     _file_size(const char *path);
uint16_t    _stat_mode(const char* path);


#define fdopen _fdopen
#define ftelli64 _ftelli64
#define fseeki64 _fseeki64
#define fileno _fileno
#else

#if !defined(__APPLE__) && !defined(__ANDROID__)
#include <sys/io.h>
#else
#define _FILE_OFFSET_BITS 64
#endif
#include  <unistd.h>
#include  <limits.h>
#include  <fcntl.h>
#include  <unistd.h>
#include  <errno.h>

#if !defined(__APPLE__) && !defined(__ANDROID__)
    #define _canonicalize_file_name(path)   canonicalize_file_name(path)
    #define _current_path()                 get_current_dir_name()
#else
    #define _canonicalize_file_name(path)   realpath(path,0)
    #define _current_path()                 getcwd(0,0)
#endif

char *       _full_path(const char *path);//need user call free(),do not check file exists
int64_t      _file_size(const char *path);
uint16_t     _stat_mode(const char* path);
char*        _exe_path();//need user call free()


#define _access(path,mode)           access(path,mode)
#define _real_path(path,resolved_path)    realpath(path,resolved_path)

#define _mkdir(path)        mkdir(path,0)
#define _rmdir(path)        rmdir(path)
#define _unlink(path)       unlink(path)

#define SH_DENYWR 0
#define SH_DENYNO 0
#define O_BINARY 0
#define O_SEQUENTIAL 0
#define O_RANDOM 0
#define O_TEMPORARY O_TMPFILE

#define ftelli64 ftello64
#define fseeki64 fseeko64
#endif
