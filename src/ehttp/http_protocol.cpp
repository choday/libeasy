#include "http_protocol.hpp"
#include <stdio.h>
namespace ehttp
{

    http_protocol::http_protocol()
    {
        major_ver = 1;
        minor_ver = 1;
        chunked=false;
        keep_alive=true;
        upgrade=false;

        _http_protocol_callback = 0;
    }

    http_protocol::~http_protocol()
    {

    }

    void http_protocol::reset()
    {
        http_parser_wrap::reset();

        major_ver = 1;
        minor_ver = 1;
        chunked=false;
        keep_alive=true;
        upgrade=false;

        headers.clear();
        
    }

    void http_protocol::make_headers(ebase::buffer& buffer)
    {




#define APPEND_BUFFER( value_name,field_string )\
        if(value_name.length())\
        {\
            char p[] = field_string ": ";\
            buffer.append( p,sizeof(p)-1 );\
            buffer.append( value_name.data(),value_name.length() );\
            buffer.append( "\r\n",2 );\
        }

        APPEND_BUFFER(upgrade_protocol,"Upgrade");
        if(upgrade_protocol.length())connection="Upgrade";

        if(minor_ver>0&&!connection.size())
        {
            if(!keep_alive)connection="Close";
            //else connection= "Keep-Alive";
        }

        APPEND_BUFFER(connection,"Connection");

        APPEND_BUFFER(content_encoding,"Content_Encoding");

#undef APPEND_BUFFER

        if(chunked)
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

    bool http_protocol::has_content_length()
    {
        return (this->_http_parser.flags&F_CONTENTLENGTH)!=0;
    }

    uint64_t http_protocol::get_content_length()
    {
        return _http_parser.content_length;
    }

    bool http_protocol::is_end()
    {
        return this->state==state_on_message_complete;
    }

    void http_protocol::add_header(const ebase::string& value)
    {
        if(0 == value.compare_size_ignore_case("connection",10 ) )
        {
            connection = find_header_value(value.c_str());
        }else if(0 == value.compare_size_ignore_case("Upgrade",7) )
        {
            upgrade_protocol = find_header_value(value.c_str());
        }else if(0 == value.compare_size_ignore_case("Content-Encoding",16) )
        {
            content_encoding = find_header_value(value.c_str());
        }else headers.push(value);
    }
    
    void http_protocol::add_header(const ebase::string& name,const ebase::string& value)
    {
        ebase::string header = name;
        header += ": ";
        header += value;

        add_header(header);
    }

    void http_protocol::on_http_parser_add_header(const ebase::string& value)
    {
        add_header(value);        
    }

    bool http_protocol::on_http_parser_headers_complete()
    {
        major_ver = http_parser_wrap::_http_parser.http_major;
        minor_ver = http_parser_wrap::_http_parser.http_minor;
        upgrade=http_parser_wrap::_http_parser.upgrade;

        int flags = http_parser_wrap::_http_parser.flags;

        chunked = (flags&F_CHUNKED)!=0;
        keep_alive= (flags&F_CONNECTION_KEEP_ALIVE)!=0;
        upgrade= (flags&F_CONNECTION_UPGRADE)!=0;

        if(_http_protocol_callback)return _http_protocol_callback->on_http_protocol_headers_complete();

        return true;
    }

    const char* http_protocol::find_header_value(const char* header)
    {
        header = strchr( header,':');
        if(!header)return header;

        for( ++header;*header && *header!=' ';++header)
        {

        }
        return header;
    }

};