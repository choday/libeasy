#include "http_url.hpp"
#include "c/http_parser.h"

namespace ehttp
{


    http_url::http_url(const char* p/*=0*/,size_t len/*=0*/,bool is_connect/*=false*/)
    {
        if(p)parse(p,len,is_connect);
    }

    http_url::~http_url()
    {

    }

    bool http_url::parse(const char* p,size_t len,bool is_connect)
    {
        if(0==len)len=strlen(p);

        http_parser_url url = {0};

        schema.resize(0);
        host.resize(0);
        port.resize(0);
        path.resize(0);
        query.resize(0);
        fragment.resize(0);
        userinfo.resize(0);

        int ret = http_parser_parse_url( p,len,is_connect,&url );
        if(ret)return false;

        if(url.field_set&(1<<UF_SCHEMA))
        {
            schema.assign(p+url.field_data[UF_SCHEMA].off,url.field_data[UF_SCHEMA].len );
            schema.make_lower();
        }

        if(url.field_set&(1<<UF_HOST))
        {
            host.assign(p+url.field_data[UF_HOST].off,url.field_data[UF_HOST].len );
        }

        if(url.field_set&(1<<UF_PORT))
        {
            port.assign(p+url.field_data[UF_PORT].off,url.field_data[UF_PORT].len );
        }

        if(url.field_set&(1<<UF_PATH))
        {
            path.assign(p+url.field_data[UF_PATH].off,url.field_data[UF_PATH].len );
        }else
        {
            path="/";
        }
        if(url.field_set&(1<<UF_QUERY))
        {
            query.assign(p+url.field_data[UF_QUERY].off,url.field_data[UF_QUERY].len );
        }
        if(url.field_set&(1<<UF_FRAGMENT))
        {
            fragment.assign(p+url.field_data[UF_FRAGMENT].off,url.field_data[UF_FRAGMENT].len );
        }
        if(url.field_set&(1<<UF_USERINFO))
        {
            userinfo.assign(p+url.field_data[UF_USERINFO].off,url.field_data[UF_USERINFO].len );
        }

        return true;
    }

    ebase::string http_url::to_string(bool include_fragment)
    {
        ebase::string result;
        
        if(0==host.length())return result;

        if(schema.length())result.append(schema);else result.append( "http" );
        
        result.append("://");
        
        if(userinfo.length())
        {
            result.append(userinfo);
            result.append("@");
        }
        result.append(host);
        if(port.length())
        {
            result.append(":");
            result.append(port);
        }

        if(path.length())
        {
            result.append(path);
        }

        if(query.length())
        {
            result.append("?");
            result.append(query);
        }

        if(include_fragment && fragment.length() )
        {
            result.append("#");
            result.append(fragment);
        }
        return result;
    }

};