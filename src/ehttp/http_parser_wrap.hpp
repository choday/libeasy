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

        int                     major_ver;
        int                     minor_ver;
        bool                    chunked;//Transfer-Encoding: chunked
        bool                    keep_alive;//keepalive tcp for http 1.1
        bool                    upgrade;
        bool                    trailer;

        void                    reset();

        enum states
        {
            state_null=0,
#define DEF_SETTINGS_FN1(fn) state_##fn,
#define DEF_SETTINGS_FN3(fn) state_##fn,
    DEF_SETTINGS_FN1(on_message_begin)
    DEF_SETTINGS_FN3(on_url)
    DEF_SETTINGS_FN3(on_status)
    DEF_SETTINGS_FN3(on_header_field)
    DEF_SETTINGS_FN3(on_header_value)
    DEF_SETTINGS_FN1(on_headers_complete)
    DEF_SETTINGS_FN3(on_body)
    DEF_SETTINGS_FN1(on_message_complete)
    DEF_SETTINGS_FN1(on_chunk_header)
    DEF_SETTINGS_FN1(on_chunk_complete)
#undef DEF_SETTINGS_FN1
#undef DEF_SETTINGS_FN3
        }                       state;

        ///有content-length或者当前为chunk-body时，body_length代表http body 长度，处于parser chunked header 阶段和http协议结束为0,其它情况 为-1
        uint64_t                body_length;
/**@brief 分析http数据
@details 
@param[in] data 
@param[in] len 数据长度,必须传入 一个大于0的数，否则parse函数会返回0
@return 返回已处理数据长度
@retval 0 出错或者解析结束，需要调用get_error_code查看具体原因
@note 每次调用parse函数只会处理一行，或者body_length的数据,所以需要反复调用parse函数
*/
        size_t                  parse_http( const char *data,size_t len );

        bool                    is_header_completed();
        bool                    is_parse_completed();

        int                     get_error_code();
        const char*             get_error_name();
        const char*             get_error_message();

        class callback
        {
        public:
            virtual inline void            on_http_parser_begin(){};
            virtual inline void            on_http_parser_end(){};
            virtual inline void            on_http_parser_headers_complete(){};
            virtual inline void            on_http_parser_chunk_header(){};
            virtual inline void            on_http_parser_chunk_complete(){};
        };

        inline void set_callback(callback* c){_callback=c;};
     protected:
        callback*               _callback;
        http_parser             _http_parser;
        http_parser_settings    _http_parser_settings;

        enum
        {
            header_cache_none,
            header_cache_url,
            header_cache_status,
            header_cache_field,
            header_cache_value,
        }                       _header_cache_type;
        ebase::string           _header_cache;

        void                    _flush_header_cache();



        virtual inline void            on_http_parser_set_path( enum http_method method,const ebase::string& value ){};
        virtual inline void            on_http_parser_set_status( int status_code,const ebase::string& value ){};
        virtual inline void            on_http_parser_add_header( const ebase::string& value ){};





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