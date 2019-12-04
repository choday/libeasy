#include "log.hpp"
#include <stdio.h>

#ifdef __ANDROID__
#include <android/log.h>
#endif



namespace ebase
{
    const char* global_log_level_name[] = 
    {
        "verbose",
        "debug",
        "info",
        "warn",
        "error",
    };

    void log::vprint(enum LOG_LEVEL level,const char* module,const char* fmt,va_list argptr)
    {
#ifdef __ANDROID__
        int prio=level+1;
        __android_log_vprint( prio,module,fmt,argptr );
#endif

        if(level>=LOG_LEVEL_SILENT-1)level=(LOG_LEVEL)(LOG_LEVEL_SILENT-1);
        if(level<=LOG_LEVEL_DEFAULT)level = LOG_VERBOSE;
        
        printf("[%s][%s]",global_log_level_name[level],module);
        vprintf( fmt,argptr );
        
    }

};