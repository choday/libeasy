#include "http_request.hpp"
namespace ehttp
{

    http_request::http_request()
    {
        accept_encoding = "gzip, deflate";
        method="GET";
    }

    http_request::~http_request()
    {

    }

    bool http_request::add_header(const ebase::string& value)
    {
        if(0 == value.compare_size_ignore_case("cookie",6) )
        {
            cookie = find_header_value(value.c_str());
        }else if(0 == value.compare_size_ignore_case("Referer",7) )
        {
            referer = find_header_value(value.c_str());
        }else if(0 == value.compare_size_ignore_case("Authorization",13) )
        {
            authorization = find_header_value(value.c_str());
        }else if(0 == value.compare_size_ignore_case("Accept-Encoding",15) )
        {
            accept_encoding = find_header_value(value.c_str());
        }else if(0 == value.compare_size_ignore_case("user-agent",10) )
        {
            user_agent = http_protocol::find_header_value( value.c_str() );
        }else if(0 == value.compare_size_ignore_case("host",4) )
        {
            host = http_protocol::find_header_value( value.c_str() );
        }else
        {
            return http_protocol::add_header(value);
        }
        return true;
    }



    ebase::buffer http_request::make_buffer()
    {
        ebase::buffer buffer;

        method.make_upper();

        if(!host.length())
        {
            host = url.host;
            if(url.port.length())
            {
                host.append(":");
                host.append(url.port);
            }
        }

        buffer.append( method.data(),method.length() );
        buffer.append( " ",1 );
        buffer.append( url.path.data(),url.path.length() );
        buffer.append( " ",1 );

        http_protocol::make_version(buffer);
        buffer.append( "\r\n",2 );

#define APPEND_BUFFER( value_name,field_string )\
        if(value_name.length())\
        {\
            char p[] = field_string ": ";\
            buffer.append( p,sizeof(p)-1 );\
            buffer.append( value_name.data(),value_name.length() );\
            buffer.append( "\r\n",2 );\
        }


        APPEND_BUFFER(host,"Host");
        APPEND_BUFFER(authorization,"Authorization");
        APPEND_BUFFER(accept_encoding,"Accept-Encoding");

        http_protocol::make_headers(buffer);


        APPEND_BUFFER(referer,"Referer");
        APPEND_BUFFER(referer,"cookie");
        APPEND_BUFFER(user_agent,"User-Agent");

        
#undef APPEND_BUFFER

        buffer.append( "\r\n",2 );//end

        return buffer;
    }

};