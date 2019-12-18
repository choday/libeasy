#pragma once

#include "../ebase/string.hpp"
#include "../ebase/buffer.hpp"
#include "http_protocol.hpp"

namespace ehttp
{

    class http_response:public http_protocol
    {
    public:
        http_response();

        virtual bool            add_header(const ebase::string& value) override;
        void                    set_status(int code,const ebase::string& msg=ebase::string());

        int                     status_code;
        ebase::string           status_message;
        ebase::string           server;//Server: nginx


        virtual  ebase::buffer  make_buffer() override;
    private:
        virtual  void            on_http_parser_set_status( int status_code,const ebase::string& value ) override;
    };

};