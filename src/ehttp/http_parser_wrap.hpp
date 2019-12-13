#pragma once

#include "c/http_parser.h"
#include "../ebase/string_view.hpp"
#include "../ebase/string.hpp"

namespace ehttp
{
    class http_parser_wrap
    {
    public:
        http_parser_wrap();
        ~http_parser_wrap();

        void                    reset();

        size_t                  parse( const char *data,size_t len );

        bool                    is_http_body_completed();
        int                     get_error_code();
        const char*             get_error_name();
        const char*             get_error_message();

     protected:
        http_parser             _http_parser;
        http_parser_settings    _http_parser_settings;

        ebase::string           _cache_value;

        void                    _flush_notify();

        virtual inline void            on_http_parser_begin(){};
        virtual inline void            on_http_parser_end(){};

        virtual inline void            on_http_parser_set_path( enum http_method method,const ebase::string& value ){};
        virtual inline void            on_http_parser_set_status( int status_code,const ebase::string& value ){};
        virtual inline void            on_http_parser_add_header( const ebase::string& value ){};
        virtual inline bool            on_http_parser_headers_complete(){return true;};//return false to skip body

        virtual inline void            on_http_parser_body( const char* data,size_t len ){}
        virtual inline void            on_http_parser_chunk_header(){};
        virtual inline void            on_http_parser_chunk_complete(){};

        enum states
        {
            state_null,
            state_on_message_begin,
            state_on_url,
            state_on_status,
            state_on_header_field,
            state_on_header_value,
            state_on_headers_complete,
            state_on_body,
            state_on_message_complete,
            state_on_chunk_header,
            state_on_chunk_complete,
        } state;

#define DEF_SETTINGS_FN1(fn) static int http_parser_wrap_##fn (http_parser*);int http_##fn();
#define DEF_SETTINGS_FN3(fn) static int http_parser_wrap_##fn (http_parser*, const char *at, size_t length);int http_##fn(const char *at, size_t length);
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
    };
};