#pragma once
extern "C"
{
typedef struct ssl_ctx_st SSL_CTX;
typedef struct ssl_st SSL;
typedef struct bio_st BIO;
}
#include "../../ebase/ref_class.hpp"
#include "ssl_context.hpp"
namespace eio
{
    class ssl_socket;
    class openssl_context:public ebase::ref_class<ssl_context>
    {
    public:
        openssl_context();
        ~openssl_context();

        virtual ebase::ref_ptr<ssl_socket>  create_ssl_socket() override;
        virtual bool load_certificate_file(const char* file,bool pem=true) override;
        virtual bool load_privatekey_file(const char* file,bool pem=true) override;
        virtual bool check_private_key() override;
    private:
        friend class ssl_socket;
        SSL_CTX*    ctx;
    };
};