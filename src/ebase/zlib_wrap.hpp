#pragma once

#include "buffer.hpp"
//https://blog.csdn.net/dj0379/article/details/52768700

extern "C" typedef struct z_stream_s z_stream;

namespace ebase
{
    class zlib_wrap
    {
    public:
        zlib_wrap();
        ~zlib_wrap();

        //bool        text_mode;//是否为二进制数据，设置此项可更好地压缩数据
    
        enum deflate_format
        {
            deflate_format_raw=0,//deflate 数据
            deflate_format_zlib,//zlib头
            deflate_format_gzip,//gzip头
        };

        void        clear();
        bool        init(bool compress,deflate_format type=deflate_format_raw );
        void        end();
        bool        write(const ebase::buffer& data);
        bool        flush(bool is_finish=false);

        bool        read(ebase::buffer& data);

        int         get_nread_size();
        inline void        set_read_cache_size(int size=4096){_read_cache_size=size;};
        inline bool        is_inited(){return zlib_stream!=0;}
    private:
        z_stream*         zlib_stream;
        int             _total_out;
        int             _error_code;
        bool            _is_compress;
        int             _read_cache_size;
        ebase::buffer   _cache_in;
        ebase::buffer   _cache_out;
        ebase::buffer_list  _cache_read;
    };
};