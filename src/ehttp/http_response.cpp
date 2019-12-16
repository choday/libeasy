#include "http_response.hpp"
#include "c/http_parser.h"

namespace ehttp
{

    http_response::http_response()
    {
        status_code=200;
        status_message="OK";
    }

    void http_response::add_header(const ebase::string& value)
    {
        if(0 == value.compare_size_ignore_case("server",4) )
        {
            server = http_protocol::find_header_value( value.c_str() );
        }else
        {
            http_protocol::add_header(value);
        }
    }

    void http_response::set_status(int code,const ebase::string& msg/*=ebase::string()*/)
    {
        status_code = code;
        status_message = msg;
    }

    ebase::buffer http_response::make_buffer()
    {
        ebase::buffer buffer;

        http_protocol::make_version(buffer);
        buffer.append( " ",1 );

        {
            ebase::string temp;
            temp.fomart_assign("%u ",status_code );
            buffer.append( temp.data(),temp.length() );

            buffer.append( status_message.data(),status_message.length() );
            buffer.append( "\r\n",2 );
        }

        http_protocol::make_headers(buffer);

#define APPEND_BUFFER( value_name,field_string )\
        if(value_name.length())\
        {\
            char p[] = field_string ": ";\
            buffer.append( p,sizeof(p)-1 );\
            buffer.append( value_name.data(),value_name.length() );\
            buffer.append( "\r\n",2 );\
        }

        if(!http_protocol::chunked)
        {
            ebase::string content_length;
            content_length="0";
            APPEND_BUFFER( content_length,"Content-Length" );
        }

        APPEND_BUFFER(server,"Server");
#undef APPEND_BUFFER

        buffer.append( "\r\n",2 );
        return buffer;
    }

    void http_response::on_http_parser_set_status(int code,const ebase::string& value)
    {
        this->set_status( code,value );
    }

};