#include "ssl_socket.hpp"
#include "openssl_context.hpp"
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

    ssl_socket::ssl_socket(openssl_context* ctx)
    {
        if(0==ctx)ctx=new openssl_context();
        _context.reset(ctx);
        this->_ssl = SSL_new(ctx->ctx);
        SSL_set_verify(this->_ssl,SSL_VERIFY_NONE,0 );//SSL_VERIFY_PEER
        this->_rbio = BIO_new(BIO_s_mem());
        this->_wbio = BIO_new(BIO_s_mem());
        SSL_set_bio(this->_ssl, this->_rbio, this->_wbio);
    }

    ssl_socket::~ssl_socket()
    {
        BIO_free(_wbio);
        BIO_free(_rbio);
        SSL_free(_ssl);
    }

    void ssl_socket::ssl_set_connect()
    {
        SSL_set_connect_state(_ssl);
        this->ssl_do_handshake();
    }

    void ssl_socket::ssl_set_accept()
    {
        SSL_set_accept_state(_ssl);
        this->ssl_do_handshake();
    }

    bool ssl_socket::ssl_do_handshake()
    {
        if(!_next_socket_io.valid() || !_next_socket_io->is_opened() )return false;

        if(SSL_is_init_finished(_ssl))return true;

        int n;
        do 
        {
            n = SSL_do_handshake(_ssl);
            if(n>0)
            {
                socket_io::notify_opened(this);
                return true;
            }

        } while (!do_error(n));

        return false;
    }

    bool ssl_socket::ssl_set_host_name(const char* name)
    {
        SSL_set_tlsext_host_name(_ssl,name);
        return true;
    }

    bool ssl_socket::open(const ebase::string& host,const ebase::string& port_or_service)
    {
        ssl_set_connect();
        ssl_set_host_name(host.c_str());
        return socket_io::open(host,port_or_service);
    }

    bool ssl_socket::open(const socket_address& address)
    {
        ssl_set_connect();

        return socket_io::open(address);
    }

    bool ssl_socket::is_opened()
    {
        return SSL_is_init_finished(_ssl);
    }

    bool ssl_socket::send(const ebase::buffer& data)
    {
        if(!do_flush())return false;

        int n=SSL_write( _ssl,data.data(),data.size() );
        if( n == data.size())return true;

        do_error(n);

        return false;
    }

    bool ssl_socket::recv(ebase::buffer& outdata)
    {
        void* p=outdata.resize(1024,false);

        do_fetch();

        int n=SSL_read( _ssl,p,outdata.capacity() );
        if(n>0)
        {
            outdata.resize(n,true);
            return true;
        }
        outdata.resize(0,true);

        do_error(n);

        return false;
    }

    int ssl_socket::get_error_code() const
    {
        if(_error.code)return _error.code;
        return _next_socket_io->get_error_code();
    }

    const char* ssl_socket::get_error_message() const
    {
        if(_error.code)return _error.message.c_str();
        return _next_socket_io->get_error_message();
    }

    bool ssl_socket::do_error(int n)
    {
        int result = SSL_get_error(_ssl,n);
        if(SSL_ERROR_WANT_READ==result)
        {
            return do_flush();
        }else if(SSL_ERROR_WANT_WRITE==result)
        {
            return do_fetch();
        }else
        {
            char buffer[256];
            unsigned long code = ERR_get_error();
            char* p = ERR_error_string(code,buffer );
            if(!p)
            {
                snprintf(buffer,256,"unkown openssl error code:%u",code);
                p=buffer;
            }

            _error.set_user_error( code,p );
            socket_io::notify_error(this);
        }
        return true;
    }

    bool ssl_socket::do_flush()
    {
        if(!_wbio)return true;

        int total = 0;
        do
        {
            if( _cache_write_buffer.size() && !this->_next_socket_io->send( _cache_write_buffer ))return false;

            void* p=_cache_write_buffer.resize(1024-ebase::buffer::header_size,false);

            int n = BIO_read( _wbio, p, _cache_write_buffer.capacity() );
            if(n>0)
            {
                _cache_write_buffer.resize(n,true);
                total+=n;
            }else
            {
                _cache_write_buffer.resize(0);
            }
        }while(_cache_write_buffer.size());

        return true;
    }

    bool ssl_socket::do_fetch()
    {
        if(!_rbio)return true;

        int total=0;
        do 
        {
            if(_cache_read_buffer.size() )
            {
                int n = BIO_write(_rbio, _cache_read_buffer.data(),_cache_read_buffer.size());
                if(n<=0)return false;
                total+=n;
            }

            _cache_read_buffer.resize(1024-ebase::buffer::header_size);
        } while(this->_next_socket_io->recv(_cache_read_buffer));

        return true;
    }

    void ssl_socket::notify_opened(ref_class_i* fire_from_handle)
    {
        ssl_do_handshake();
    }

    void ssl_socket::notify_readable(ref_class_i* fire_from_handle)
    {
        do_fetch();
        if(ssl_do_handshake())socket_io::notify_readable(this);
    }

    void ssl_socket::notify_writeable(ref_class_i* fire_from_handle)
    {
        do_flush();
        if(ssl_do_handshake())socket_io::notify_writeable(this);
    }
};