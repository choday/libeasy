#include "http_protocol.hpp"
#include <stdio.h>
#include <stdlib.h>

namespace ehttp
{

    http_protocol::http_protocol()
    {
        content_length=-1;

        major_ver = 1;
        minor_ver = 1;
        chunked=false;
        keep_alive=true;
        upgrade=false;

    }

    http_protocol::~http_protocol()
    {

    }

    void http_protocol::reset()
    {
        http_parser_wrap::reset();

        content_length=-1;

        major_ver = 1;
        minor_ver = 1;
        chunked=false;
        keep_alive=true;
        upgrade=false;

        headers.clear();
        
    }

    void http_protocol::make_headers(ebase::buffer& buffer)
    {
        if(minor_ver<1)keep_alive = false;


#define APPEND_BUFFER( value_name,field_string )\
        if(value_name.length())\
        {\
            char p[] = field_string ": ";\
            buffer.append( p,sizeof(p)-1 );\
            buffer.append( value_name.data(),value_name.length() );\
            buffer.append( "\r\n",2 );\
        }

        APPEND_BUFFER(upgrade_protocol,"Upgrade");
        if(upgrade_protocol.length())
        {
            upgrade = true;
            connection="Upgrade";
            keep_alive = true;
            if(minor_ver<1)minor_ver=1;
        }

        if(minor_ver>0&&!connection.size())
        {
            if(!keep_alive)connection="Close";
            else connection= "Keep-Alive";
        }

        APPEND_BUFFER(connection,"Connection");

        if(!upgrade_protocol.length()&&!chunked&&content_length!=-1)
        {
            ebase::string strcontent_length;
            strcontent_length.fomart_assign("%llu",content_length);
            APPEND_BUFFER( strcontent_length,"Content-Length" );
        }
      
        APPEND_BUFFER(content_encoding,"Content-Encoding");

#undef APPEND_BUFFER
        if(chunked&&minor_ver>0)
        {
            char p[] = "Transfer-Encoding: chunked\r\n";
            buffer.append( p,sizeof(p)-1 );
        }
        for(int i = 0;i<headers.size();++i)
        {
            const ebase::string& data = headers.at(i);
            buffer.append( data.data(),data.length() );
            buffer.append( "\r\n",2 );
        }
    }

    void http_protocol::make_version(ebase::buffer& buffer)
    {
        char ver[] = "HTTP/0.0";
        ver[5] += major_ver;
        ver[7] += minor_ver;
        buffer.append( ver,8 );
    }


    bool http_protocol::add_header(const ebase::string& value)
    {
        if(0 == value.compare_size_ignore_case("connection",10 ) )
        {
            connection = find_header_value(value.c_str());
        }else if(0 == value.compare_size_ignore_case("Upgrade",7) )
        {
            upgrade_protocol = find_header_value(value.c_str());
        }else if(0 == value.compare_size_ignore_case("Content-Length",15) )
        {
            const char* p = find_header_value(value.c_str());
            if(!p)p="0";
            content_length = _strtoi64(p,0,10);
        }else if(0 == value.compare_size_ignore_case("Content-Encoding",16) )
        {
            content_encoding = find_header_value(value.c_str());
        }else headers.push(value);
        return true;
    }
    
    bool http_protocol::add_header(const ebase::string& name,const ebase::string& value)
    {
        ebase::string header = name;
        header += ": ";
        header += value;

        return add_header(header);
    }

    void http_protocol::on_http_parser_add_header(const ebase::string& value)
    {
        add_header(value);        
    }

    const char* http_protocol::find_header_value(const char* header)
    {
        header = strchr( header,':');
        if(!header)return header;

        ++header;

        while(*header)
        {
            if(*header!=' ')break;
            ++header;
        }

        return header;
    }

};