#include "http_parser_wrap.hpp"
#include "../ebase/allocator.hpp"
#include <stdio.h>

namespace ehttp
{
    

    http_parser_wrap::http_parser_wrap()
    {

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
    }

    http_parser_wrap::~http_parser_wrap()
    {

    }

    size_t http_parser_wrap::parse(const char *data,size_t len)
    {
        if(state_on_message_complete == this->state)return 0;
        size_t result = http_parser_execute( &_http_parser,&_http_parser_settings,data,len );
        return result;
    }



    bool http_parser_wrap::is_http_body_completed()
    {
        return http_body_is_final(&_http_parser)!=0;
    }

    const char* http_parser_wrap::get_error_name()
    {
        return http_errno_name((http_errno)_http_parser.http_errno);
    }

    const char* http_parser_wrap::get_error_message()
    {
        return http_errno_description((http_errno)_http_parser.http_errno);
    }


    void http_parser_wrap::_flush_notify()
    {
        switch(this->state)
        {

        case state_on_url:
            this->on_http_parser_set_path((enum http_method)this->_http_parser.method,_cache_value);
            _cache_value.resize(0);
            break;
        case state_on_status:
            this->on_http_parser_set_status(this->_http_parser.status_code,_cache_value);
            _cache_value.resize(0);
            break;
        case state_on_header_field:
            _cache_value.append(": ",2);
            break;
        case state_on_header_value:
            this->on_http_parser_add_header(_cache_value);
            _cache_value.resize(0);
            break;
        }
    }

#define DEF_SETTINGS_FN1(fn) int http_parser_wrap::http_parser_wrap_##fn (http_parser* parser){http_parser_wrap* pthis=(http_parser_wrap*)parser->data;return pthis->http_##fn();}int http_parser_wrap::http_##fn()
#define DEF_SETTINGS_FN3(fn) int http_parser_wrap::http_parser_wrap_##fn (http_parser* parser, const char *at, size_t length){http_parser_wrap* pthis=(http_parser_wrap*)parser->data;return pthis->http_##fn(at,length);}int http_parser_wrap::http_##fn(const char *at, size_t length)
    DEF_SETTINGS_FN1(on_message_begin)
    {
        _cache_value.resize(0);
        this->state = state_on_message_begin;
        this->on_http_parser_begin();
        return 0;
    }

    DEF_SETTINGS_FN3(on_url){

        if(length)_cache_value.append( at,length);
        this->state = state_on_url;
        return 0;
    }

    DEF_SETTINGS_FN3(on_status)
    {
        if(length)_cache_value.append( at,length);

        this->state = state_on_status;
        return 0;
    }

    DEF_SETTINGS_FN3(on_header_field)
    {
        _flush_notify();

        if(length)_cache_value.append( at,length);

        this->state = state_on_header_field;
        return 0;
    }

    DEF_SETTINGS_FN3(on_header_value)
    {
        _flush_notify();

        if(length)_cache_value.append( at,length);

        this->state = state_on_header_value;
        return 0;
    }

    DEF_SETTINGS_FN1(on_headers_complete)
    {
        _flush_notify();
        this->state = state_on_headers_complete;
        if(this->on_http_parser_headers_complete())return 0;
        else return 1;        
    }

    DEF_SETTINGS_FN3(on_body)
    {
        this->state = state_on_body;
        this->on_http_parser_body(at,length);
        return 0;
    }

    DEF_SETTINGS_FN1(on_message_complete)
    {
        this->state = state_on_message_complete;
        this->on_http_parser_end();
        return 0;
    }

    DEF_SETTINGS_FN1(on_chunk_header){
        this->state = state_on_chunk_header;
        this->on_http_parser_chunk_header();
        return 0;
    }
    DEF_SETTINGS_FN1(on_chunk_complete){
        this->state = state_on_chunk_complete;
        this->on_http_parser_chunk_complete();
        return 0;
    }
#undef DEF_SETTINGS_FN3
#undef DEF_SETTINGS_FN1
}