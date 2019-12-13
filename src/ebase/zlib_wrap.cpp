#include "zlib_wrap.hpp"
#include "allocator.hpp"

#include <assert.h>
#include <zlib.h>
namespace ebase
{
//Î´Íê³É
    zlib_wrap::zlib_wrap()
    {
        _read_cache_size=4096;
        _error_code=0;
        _total_out=0;
        zlib_stream=0;
    }

    zlib_wrap::~zlib_wrap()
    {
        clear();
    }

    void zlib_wrap::clear()
    {
        if(zlib_stream)end();

        zlib_stream=0;
        _total_out=0;
        _error_code = 0;
        _is_compress =false;
        _read_cache_size=4096;
        _cache_in.resize(0);
        _cache_out.resize(0);
        _cache_read.clear();

    }

    bool zlib_wrap::init(bool compress,deflate_format type)
    {

        if(zlib_stream)return false;
        zlib_stream = (z_stream*)ebase::allocator::malloc(sizeof(z_stream));
        zlib_stream->zalloc = (alloc_func)0;
        zlib_stream->zfree = (free_func)0;
        zlib_stream->opaque = (voidpf)0;

        _is_compress=compress;

        int result = 0;

        if(type==deflate_format_zlib)
        {
            if(compress)result = deflateInit(zlib_stream, Z_DEFAULT_COMPRESSION);
            else result = inflateInit(zlib_stream);
        }else if(type==deflate_format_raw)
        {
            //if(compress)result = deflateInit(zlib_stream, Z_DEFAULT_COMPRESSION);
            if(compress) result = deflateInit2(zlib_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,-MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
            else result = inflateInit2(zlib_stream,-MAX_WBITS);
        }else if(type==deflate_format_gzip)
        {
            if(compress) result = deflateInit2(zlib_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY);
            else result = inflateInit2(zlib_stream,MAX_WBITS + 16);;//wbits=47???
        }

        zlib_stream->next_in = 0;
        zlib_stream->avail_in = 0;
        zlib_stream->next_out = 0;
        zlib_stream->avail_out = 0;

        return Z_OK==result;
    }

    void zlib_wrap::end()
    {
        if(0==zlib_stream)return;
        flush(true);
        if(_is_compress)deflateEnd(zlib_stream);
        else inflateEnd(zlib_stream);
        ebase::allocator::free(zlib_stream);
        zlib_stream=0;

    }

    bool zlib_wrap::write(const ebase::buffer& data)
    {
        if(_error_code)return false;
        if(!zlib_stream)return false;

        if(_cache_in.size() )
        {
            if(!flush()) return false;
        }

        assert(0==_cache_in.size() );

        _cache_in = data;
        
        zlib_stream->next_in = (z_const Bytef *)_cache_in.data();
        zlib_stream->avail_in = _cache_in.size();

        flush();

        return true;
    }

    bool zlib_wrap::flush(bool is_finish)
    {
        if(!zlib_stream)return false;

        if(!is_finish&&get_nread_size()>_read_cache_size)return false;

        uLong total_out=zlib_stream->total_out;
        int result=Z_OK;
        do
        {
            if(0==zlib_stream->avail_in)
            {
                _cache_in.resize(0);
                if(!is_finish)break;
            }

            if(0==zlib_stream->avail_out)
            {
                if(zlib_stream->total_out>_total_out)
                {
                    _cache_out.resize(zlib_stream->total_out-_total_out);
                    _cache_read.push_back(_cache_out);
                    _cache_out.resize(0);
                    _total_out=zlib_stream->total_out;
                }

                if(!is_finish&&_cache_read.data_size()>=_read_cache_size)break;

                int capacity = _cache_out.capacity();
                if(!capacity)capacity= 1024-ebase::buffer::header_size;

                int old_size = _cache_out.size();
                char* p = (char*)_cache_out.grown(capacity-old_size);

                zlib_stream->next_out = (Bytef *)p;
                zlib_stream->avail_out = capacity-old_size;
            }

            if(_is_compress)result = deflate(zlib_stream, (zlib_stream->avail_in==0&&is_finish)?Z_FINISH:Z_NO_FLUSH );
            else result = inflate(zlib_stream, (zlib_stream->avail_in==0&&is_finish)?Z_FINISH:Z_NO_FLUSH );

        } while (Z_OK==result);

        if(Z_STREAM_END ==result)
        {
            if(zlib_stream->total_out>_total_out)
            {
                _cache_out.resize(zlib_stream->total_out-_total_out);
                _cache_read.push_back(_cache_out);
                _cache_out.resize(0);
                _total_out=zlib_stream->total_out;
            }
            result=Z_OK;
        }

        _error_code = result;
        return Z_OK==result;
    }

    bool zlib_wrap::read(ebase::buffer& data)
    {
        if(_cache_out.size()+_cache_read.data_size() < _read_cache_size )flush();

        if(_cache_read.count())
        {
            data=_cache_read.pop_front();
            return true;
        }

        if(_cache_out.size() > 0)
        {
            data = _cache_out;
            _cache_out.resize(0);
            return true;
        }

        return false;
    }

    int zlib_wrap::get_nread_size()
    {
        return _cache_read.data_size()+_cache_out.size();
    }

};