#include "http_parser_wrap.hpp"
#include "../ebase/allocator.hpp"
#include <stdio.h>

namespace ehttp
{
    

    http_parser_wrap::http_parser_wrap()
    {
        _callback=0;

        reset();

        http_parser_settings_init(&_http_parser_settings);

#define DEF_SETTINGS_FN1(fn) _http_parser_settings.##fn = &http_parser_wrap::http_parser_wrap_##fn;
#define DEF_SETTINGS_FN3 DEF_SETTINGS_FN1
    DEF_SETTINGS_FN1(on_message_begin);
    DEF_SETTINGS_FN3(on_url);
    DEF_SETTINGS_FN3(on_status);
    DEF_SETTINGS_FN3(on_header_field);
    DEF_SETTINGS_FN3(on_header_value);
    DEF_SETTINGS_FN1(on_headers_complete);
    DEF_SETTINGS_FN3(on_body);
    DEF_SETTINGS_FN1(on_message_complete);
    DEF_SETTINGS_FN1(on_chunk_header);
    DEF_SETTINGS_FN1(on_chunk_complete);
#undef DEF_SETTINGS_FN1
#undef DEF_SETTINGS_FN3
    }

    void http_parser_wrap::reset()
    {
        http_parser_init( &_http_parser,HTTP_BOTH );
        _http_parser.data = this;

        this->state=state_null;
        this->_header_cache_type = header_cache_none;
        this->_header_cache.resize(0);
        this->body_length=0;

        major_ver = 1;
        minor_ver = 1;
        chunked=false;
        keep_alive=false;
        upgrade=false;
        trailer = false;
    }

    http_parser_wrap::~http_parser_wrap()
    {

    }

    size_t http_parser_wrap::parse_http(const char *data,size_t len)
    {
        if(len==0)return 0;
        if(state_on_message_complete == state)return 0;

        if( body_length )
        {
             if(len>body_length)len = body_length;
        }else
        {
            //ÐÐÊý¾Ý
            for( int i = 0;i<len;++i )
            {
                if( '\n' == data[i] )
                {
                    len = i+1;
                    break;
                }
            }
        }

        size_t result = http_parser_execute( &_http_parser,&_http_parser_settings,data,len );
        assert(result==len);

        return result;
    }



    bool http_parser_wrap::is_header_completed()
    {
        return state>=state_on_headers_complete;
    }

    bool http_parser_wrap::is_parse_completed()
    {
        return state_on_message_complete == state;
    }

    int http_parser_wrap::get_error_code()
    {
        return int((http_errno)_http_parser.http_errno);
    }

    const char* http_parser_wrap::get_error_name()
    {
        return http_errno_name((http_errno)_http_parser.http_errno);
    }

    const char* http_parser_wrap::get_error_message()
    {
        return http_errno_description((http_errno)_http_parser.http_errno);
    }

    void http_parser_wrap::_flush_header_cache()
    {
        switch(this->_header_cache_type)
        {
        case header_cache_url:
            this->on_http_parser_set_path((enum http_method)this->_http_parser.method,_header_cache);
            _header_cache.resize(0);
            break;
        case header_cache_status:
            this->on_http_parser_set_status(this->_http_parser.status_code,_header_cache);
            _header_cache.resize(0);
            break;
        case header_cache_field:
            _header_cache.append(": ",2);
            break;
        case header_cache_value:
            this->on_http_parser_add_header(_header_cache);
            _header_cache.resize(0);
            break;
        }
    }

#define DEF_SETTINGS_FN1(fn) int http_parser_wrap::http_parser_wrap_##fn (http_parser* parser){\
        http_parser_wrap* pthis=(http_parser_wrap*)parser->data;\
        int result = pthis->http_##fn();\
        pthis->state = state_##fn;\
        return result;\
    }\
    int http_parser_wrap::http_##fn()

#define DEF_SETTINGS_FN3(fn) int http_parser_wrap::http_parser_wrap_##fn (http_parser* parser, const char *at, size_t length){\
        http_parser_wrap* pthis=(http_parser_wrap*)parser->data;\
        int result = pthis->http_##fn(at,length);\
        pthis->state = state_##fn;\
        return result;\
    }\
    int http_parser_wrap::http_##fn(const char *at, size_t length)

    DEF_SETTINGS_FN1(on_message_begin)
    {
        _header_cache.resize(0);
        if(_callback)_callback->on_http_parser_begin();
        return 0;
    }

    DEF_SETTINGS_FN3(on_url){
        if(length)_header_cache.append( at,length);
        this->_header_cache_type = header_cache_url;
        return 0;
    }

    DEF_SETTINGS_FN3(on_status)
    {
        if(length)_header_cache.append( at,length);
        this->_header_cache_type = header_cache_status;
        return 0;
    }

    DEF_SETTINGS_FN3(on_header_field)
    {
        _flush_header_cache();

        if(length)_header_cache.append( at,length);

        this->_header_cache_type = header_cache_field;
        return 0;
    }

    DEF_SETTINGS_FN3(on_header_value)
    {
        _flush_header_cache();

        if(length)_header_cache.append( at,length);

        this->_header_cache_type = header_cache_value;
        return 0;
    }

    DEF_SETTINGS_FN1(on_headers_complete)
    {
        _flush_header_cache();

        major_ver = _http_parser.http_major;
        minor_ver = _http_parser.http_minor;
 

        chunked = (_http_parser.flags&F_CHUNKED)!=0;
        keep_alive= (_http_parser.flags&F_CONNECTION_KEEP_ALIVE)!=0;
        upgrade= (_http_parser.flags&F_CONNECTION_UPGRADE)!=0;
        trailer= (_http_parser.flags&F_TRAILING)!=0;
        
        
        if(chunked)this->body_length = 0;
        else if(F_CONTENTLENGTH&_http_parser.flags)this->body_length = this->_http_parser.content_length;
        else this->body_length = -1;

        if(_callback)_callback->on_http_parser_headers_complete();
        return 0;
    }

    DEF_SETTINGS_FN3(on_body)
    {
        this->body_length -= length;
        return 0;
    }

    DEF_SETTINGS_FN1(on_message_complete)
    {
        this->body_length = 0;
        if(_callback)_callback->on_http_parser_end();
        return 0;
    }

    DEF_SETTINGS_FN1(on_chunk_header){
        this->body_length = this->_http_parser.content_length;
        if(_callback)_callback->on_http_parser_chunk_header();
        return 0;
    }
    DEF_SETTINGS_FN1(on_chunk_complete){
        this->body_length = 0;
        if(_callback)_callback->on_http_parser_chunk_complete();
        return 0;
    }
#undef DEF_SETTINGS_FN3
#undef DEF_SETTINGS_FN1
}