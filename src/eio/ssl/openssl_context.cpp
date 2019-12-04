#include "openssl_context.hpp"
#include "ssl_socket.hpp"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#ifdef _WIN32
#define snprintf _snprintf
#endif
//https://gist.github.com/darrenjs/4645f115d10aa4b5cebf57483ec82eca
/*
  +------+                                    +-----+
  |......|--> read(fd) --> BIO_write(rbio) -->|.....|--> SSL_read(ssl)  --> IN
  |......|                                    |.....|
  |.sock.|                                    |.SSL.|
  |......|                                    |.....|
  |......|<-- write(fd) <-- BIO_read(wbio) <--|.....|<-- SSL_write(ssl) <-- OUT
  +------+                                    +-----+
          |                                  |       |                     |
          |<-------------------------------->|       |<------------------->|
          |         encrypted bytes          |       |  unencrypted bytes  |
    result = SSL_get_error(ctx,result);
    switch(result)
    {
    case SSL_ERROR_NONE:
    case SSL_ERROR_SSL:
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_X509_LOOKUP:
    case SSL_ERROR_SYSCALL:
    case SSL_ERROR_ZERO_RETURN://连接正常关闭
    case SSL_ERROR_WANT_CONNECT:
    case SSL_ERROR_WANT_ACCEPT:
    }
*/
namespace eio
{

    void init_openssl_library()
    {
        static bool inited = false;
        if(inited)return;
        inited=true;

        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        ERR_load_ERR_strings();
        ERR_load_BIO_strings();
        ERR_load_crypto_strings();
    }

    openssl_context::openssl_context()
    {
        init_openssl_library();
        ctx = SSL_CTX_new(TLSv1_2_method());
    }

    openssl_context::~openssl_context()
    {
        SSL_CTX_free(ctx);
    }

    ebase::ref_ptr<eio::ssl_socket> openssl_context::create_ssl_socket()
    {
        return new ssl_socket(this);
    }


    bool openssl_context::load_certificate_file(const char* file,bool pem)
    {
        return 1==SSL_CTX_use_certificate_file( ctx,file,pem?SSL_FILETYPE_PEM:SSL_FILETYPE_ASN1 );
    }

    bool openssl_context::load_privatekey_file(const char* file,bool pem)
    {
        return 1==SSL_CTX_use_PrivateKey_file( ctx,file,pem?SSL_FILETYPE_PEM:SSL_FILETYPE_ASN1 );
    }

    bool openssl_context::check_private_key()
    {
        return 1==SSL_CTX_check_private_key(ctx);
    }

}