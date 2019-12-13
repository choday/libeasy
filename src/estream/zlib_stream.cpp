#include "zlib_stream.hpp"
#include "../ebase/allocator.hpp"
#include <zlib.h>
namespace estream
{


    zlib_stream::zlib_stream()
    {
        _z_stream = 0;
        _is_compress=false;
        _auto_release=true;

        ebase::stream::private_on_want_push_buffer.set_function( &zlib_stream::_on_want_push_buffer,this ); 
        ebase::stream::private_on_want_pop_buffer.set_function( &zlib_stream::_on_want_pop_buffer,this ); 
    }

    zlib_stream::~zlib_stream()
    {

    }

    bool zlib_stream::init(bool compress,deflate_format type/*=deflate_format_raw */)
    {
        _cache_in.resize(0);
        _cache_out.resize(0);

        _z_stream = (z_stream*)ebase::allocator::malloc(sizeof(z_stream));

        _z_stream->zalloc = (alloc_func)0;
        _z_stream->zfree = (free_func)0;
        _z_stream->opaque = (voidpf)0;
        _z_stream->next_in = 0;
        _z_stream->avail_in = 0;
        _z_stream->next_out = 0;
        _z_stream->avail_out = 0;

        _is_compress=compress;

        int result=Z_OK;
        if(type==deflate_format_zlib)
        {
            if(compress)result = deflateInit(_z_stream, Z_DEFAULT_COMPRESSION);
            else result = inflateInit(_z_stream);
        }else if(type==deflate_format_raw)
        {
            //if(compress)result = deflateInit(zlib_stream, Z_DEFAULT_COMPRESSION);
            if(compress) result = deflateInit2(_z_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,-MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
            else result = inflateInit2(_z_stream,-MAX_WBITS);
        }else if(type==deflate_format_gzip)
        {
            if(compress) result = deflateInit2(_z_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY);
            else result = inflateInit2(_z_stream,MAX_WBITS + 16);;//wbits=47???
        }

        return Z_OK == result;
    }

    void zlib_stream::disable_auto_release()
    {
        _auto_release=false;
    }

    void zlib_stream::_on_want_push_buffer(ebase::ref_class_i* from)
    {
        if(!_z_stream)return;
        do_transform();
    }

    void zlib_stream::_on_want_pop_buffer(ebase::ref_class_i* from)
    {
        if(!_z_stream)return;
        do_transform();
    }

    void zlib_stream::do_transform()
    {
        int result=Z_OK;

        bool is_finish = ebase::stream::is_end();

        do
        {
            if(0==_z_stream->avail_in)
            {
                _cache_in.resize(0);

                if(ebase::stream::get_write_cache_size())
                {
                    _cache_in = ebase::stream::private_pop_buffer();
                    _z_stream->next_in = (Bytef *)_cache_in.data();
                    _z_stream->avail_in = _cache_in.size();

                    assert(_cache_in.size());
                }else if(!is_finish )
                {
                    break;
                }
            }

            if(0==_z_stream->avail_out)
            {
                if(_z_stream->total_out>0)
                {
                    _cache_out.resize(_z_stream->total_out);
                    ebase::stream::private_push_buffer(_cache_out);

                    _cache_out.resize(0);
                    _z_stream->total_out=0;
                }

                int capacity = _cache_out.capacity();
                if(!capacity)capacity= 1024-ebase::buffer::header_size;

                char* p = (char*)_cache_out.alloc(capacity);

                _z_stream->next_out = (Bytef *)p;
                _z_stream->avail_out = capacity;
            }

            if(_is_compress)result = deflate(_z_stream, (_z_stream->avail_in==0&&is_finish)?Z_FINISH:Z_NO_FLUSH );
            else result = inflate(_z_stream, (_z_stream->avail_in==0&&is_finish)?Z_FINISH:Z_NO_FLUSH );

        } while (Z_OK==result);


        if(Z_OK!=result)
        {
            if(Z_STREAM_END ==result)
            {
                if(_z_stream->total_out)
                {
                    _cache_out.resize(_z_stream->total_out);
                    ebase::stream::private_push_buffer(_cache_out);
                    _cache_out.resize(0);
                    _z_stream->total_out=0;
                }
                ebase::stream::private_notify_end();
            }else
            {
                ebase::stream::private_notify_error( result,_z_stream->msg );
            }

            if(_is_compress)deflateEnd(_z_stream);
            else inflateEnd(_z_stream);
            ebase::allocator::free(_z_stream);
            _z_stream=0;
        }
    }

    void zlib_stream::on_destroy()
    {
        if(_auto_release)delete this;
    }

};