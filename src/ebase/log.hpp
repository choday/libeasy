#pragma once
#include <stdarg.h>

namespace ebase
{
    enum LOG_LEVEL
    {
        LOG_LEVEL_DEFAULT=0,
        LOG_VERBOSE,//详细信息
        LOG_DEBUG,//调试信息
        LOG_INFO,//一般信息
        LOG_WARN,//警告信息
        LOG_ERROR,//错误信息
        LOG_LEVEL_SILENT,//
    };

    class log
    {
    public:


        static inline void print(enum LOG_LEVEL level,const char* module,const char* fmt,...)
                            {va_list args;va_start(args,fmt);vprint(level,module,fmt,args);va_end(args);}

        static inline void verbose(const char* module,const char* fmt,...)
                            {va_list args;va_start(args,fmt);vprint(LOG_VERBOSE,module,fmt,args);va_end(args);}
        static inline void debug(const char* module,const char* fmt,...)
                            {va_list args;va_start(args,fmt);vprint(LOG_DEBUG,module,fmt,args);va_end(args);}
        static inline void warn(const char* module,const char* fmt,...)
                            {va_list args;va_start(args,fmt);vprint(LOG_WARN,module,fmt,args);va_end(args);}
        static inline void error(const char* module,const char* fmt,...)
                            {va_list args;va_start(args,fmt);vprint(LOG_ERROR,module,fmt,args);va_end(args);}

    public:


    private:
        static void vprint(enum LOG_LEVEL level,const char* module,const char* fmt,va_list argptr);
    };
};