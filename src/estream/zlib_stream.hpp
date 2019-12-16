#pragma once
#include "../ebase/stream.hpp"
extern "C" typedef struct z_stream_s z_stream;

namespace estream
{

    class zlib_stream:public ebase::stream
    {
    public:
        zlib_stream();
        ~zlib_stream();

        enum deflate_format
        {
            deflate_format_raw=0,//deflate 数据
            deflate_format_zlib,//zlib头
            deflate_format_gzip,//gzip头
        };

        bool        init(bool compress,deflate_format type=deflate_format_raw,bool text_mode = false );

        void        disable_auto_release();
    private:
        bool              _auto_release;
        z_stream*         _z_stream;
        bool              _is_compress;

        ebase::buffer     _cache_in;
        ebase::buffer     _cache_out;

        void              _on_want_push_buffer(ebase::ref_class_i* from);
        void              _on_want_pop_buffer(ebase::ref_class_i* from);

        void             do_transform();

        virtual void		on_destroy() override;
    };

};