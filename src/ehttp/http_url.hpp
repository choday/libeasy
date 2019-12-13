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

        ebase::string   schema;//����
        ebase::string   host;//����
        ebase::string   port;
        ebase::string   path;
        ebase::string   query;
        ebase::string   fragment;//https://domain/index#L18 ,# �б��� ?��? ����Ĳ�ѯ�ַ����ᱻ����������Ϸ��������� fragment ���ᱻ���͵ķ�������
        ebase::string   userinfo;

        //[schema]://userinfo@host:port/path?query#fragment
    };
};