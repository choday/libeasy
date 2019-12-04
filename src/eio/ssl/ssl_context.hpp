#pragma once
#include "../../ebase/ref_class.hpp"
#include "../../ebase/buffer.hpp"
namespace eio
{

    class ssl_socket;
    class ssl_context:public ebase::ref_class_i
    {
    public:

        virtual ebase::ref_ptr<ssl_socket>  create_ssl_socket() = 0;

        virtual bool load_certificate_file(const char* file,bool pem=true) = 0;
        virtual bool load_privatekey_file(const char* file,bool pem=true) = 0;
        virtual bool check_private_key() = 0;
    };

    typedef ebase::ref_ptr<ssl_context>  ssl_context_ptr;
}