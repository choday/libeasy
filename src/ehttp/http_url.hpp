#pragma once
#include "../ebase/string.hpp"
#include <stdint.h>

namespace ehttp
{

    class http_url
    {
    public:
        http_url(const char* p=0,size_t len=0,bool is_connect=false);
        ~http_url();

        bool            parse(const char* p,size_t len,bool is_connect=false);
        inline bool     parse(const ebase::string& url,bool is_connect=false ){return parse(url.data(),url.length(),is_connect );}

        ebase::string   to_string(bool include_fragment=true);

        ebase::string   schema;//必填
        ebase::string   host;//必填
        ebase::string   port;
        ebase::string   path;
        ebase::string   query;
        ebase::string   fragment;//https://domain/index#L18 ,# 有别于 ?，? 后面的查询字符串会被网络请求带上服务器，而 fragment 不会被发送的服务器；
        ebase::string   userinfo;

        //[schema]://userinfo@host:port/path?query#fragment
    };
};