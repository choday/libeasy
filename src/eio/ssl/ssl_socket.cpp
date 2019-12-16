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
        _ssl_connected = false;

        _max_read_cache_size=2048;

        this->_ssl = SSL_new(ctx->ctx);
        //SSL_set_verify(this->_ssl,SSL_VERIFY_NONE,0 );//SSL_VERIFY_PEER
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


    bool ssl_socket::ssl_set_host_name(const char* name)
    {
        SSL_set_tlsext_host_name(_ssl,name);
        return true;
    }

    void ssl_socket::init(bool is_server/*=true*/)
    {
        if(is_server)
        {
            SSL_set_accept_state(_ssl);
        }else
        {
            SSL_set_connect_state(_ssl);
        }
        if(_next_socket_io.valid() && _next_socket_io->is_opened())this->do_handshake_loop();
    }

    bool ssl_socket::open(const ebase::string& host,const ebase::string& port_or_service)
    {
        init(false);
 
        return socket_io_filter::open(host,port_or_service);
    }

    bool ssl_socket::open(const socket_address& address)
    {
        init(false);

        return socket_io_filter::open(address);
    }

    bool ssl_socket::is_opened()
    {
        return 0!=SSL_is_init_finished(_ssl);
    }

    int ssl_socket::write(const void* data,int len)
    {
        if(do_flush_cache_out()<0)return -1;

        int n=SSL_write( _ssl,data,len );
        if( n > 0 )
        {
            assert(n == len);
            do_flush_cache_out();
            return n;
        }
        if( has_error(n) )return -1;

        return 0;
    }

    int ssl_socket::read(void* data,int len)
    {
        this->update_ssl_in(len);

        int n=SSL_read( _ssl,data,len );
        if(n>0)
        {
            this->update_ssl_in();
            return n;
        }

        if( has_error(n) )return -1;

        return 0;
    }

    int ssl_socket::get_nread_size() const 
    {
        if(!_ssl_connected)return 0;
        return SSL_pending(_ssl);
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

    bool ssl_socket::has_error(int n)
    {
        if(_error.code)return true;

        char buffer[256];

        unsigned long code = ERR_get_error();
        if(0==code)return false;

        buffer[0] = 0;
        ERR_error_string_n(code,buffer,sizeof(buffer) );

        _error.set_user_error( code,buffer );
        socket_io_filter::notify_error(this);
        socket_io_filter::_next_socket_io->close(true);

        return true;
    }

    int ssl_socket::do_flush_cache_out()
    {
        if(!_wbio)return -1;

        int total = 0;
        int n;

        do 
        {
            if( _out_cache.size() )
            {
                n = this->_next_socket_io->write_buffer( _out_cache );
                if(n<=0)return n;
                total += _out_cache.size();
            }

            void* p=_out_cache.alloc(1024-ebase::buffer::header_size);
            n = BIO_read( _wbio, p, _out_cache.capacity() );
            if(n>0)
            {
                _out_cache.resize(n);
            }else
            {
                _out_cache.resize(0);
            }
        } while (n>0);

        return total;
    }

    int ssl_socket::do_fetch_cache_in()
    {
        if(!_rbio)return false;

        if(!_in_cache.size() )
        {
            int capacity = _in_cache.capacity();
            if(0==capacity)capacity = 1024-ebase::buffer::header_size;
            void* p = _in_cache.alloc(capacity);
            int result=this->_next_socket_io->read(p,capacity);
            if(result<=0)
            {
                _in_cache.resize(0);
                return false;
            }else
            {
                _in_cache.resize(result);
            }
        }

        assert(_in_cache.size());

        int n = BIO_write(_rbio, _in_cache.data(),_in_cache.size());
        if(n<=0)return n;

        assert(n==_in_cache.size());
        _in_cache.resize(0);

        return n;
    }

    int ssl_socket::do_handshake_loop()
    {
        if(!_next_socket_io.valid() || !_next_socket_io->is_opened() )return -1;
        if(SSL_is_init_finished(_ssl))return 0;
        if(_ssl_connected)return 0;

        int total_size = 0;

        int n;
        do 
        {
            n = SSL_do_handshake(_ssl);
            if(n>0)
            {
                _ssl_connected=true;
                socket_io_filter::notify_opened(this);
                socket_io_filter::notify_writeable(this);
                return 0;
            }

            int result = SSL_get_error(_ssl,n);
            if(SSL_ERROR_WANT_READ==result)
            {
                n=do_flush_cache_out();
                if(n<0)return -1;

                total_size+=n;

            }else if(SSL_ERROR_WANT_WRITE==result)
            {
                
            }else
            {
                //出错
                if( has_error(n) )return -1;
            }

            n=do_fetch_cache_in();
            if(n<0)return -1;

        } while (n>0);

        return total_size;
    }

    void ssl_socket::notify_opened(ref_class_i* fire_from_handle)
    {

    }

    void ssl_socket::notify_readable(ref_class_i* fire_from_handle)
    {
        if(!_ssl_connected && 0!=do_handshake_loop() )return;

        int pendding_size = update_ssl_in();
        if(pendding_size>0)socket_io_filter::notify_readable(this);
    }
    
    void ssl_socket::notify_writeable(ref_class_i* fire_from_handle)
    {
        if(!_ssl_connected && 0!=do_handshake_loop() )return;

        if(do_flush_cache_out()>0)
        {
            socket_io_filter::notify_writeable(this);
        }
        
    }

    int ssl_socket::update_ssl_in(int need_bytes)
    {
        if(0==need_bytes)need_bytes=_max_read_cache_size;
        char buffer[4];
        int pendding_size=0;

        int n= 0;
        do
        {
            int n = SSL_peek( _ssl,buffer,4 );
            if(n>0)
            {
                pendding_size = SSL_pending(_ssl);
                if(pendding_size>_max_read_cache_size)break;
            }

            if(has_error(n))return -1;

        }while(do_fetch_cache_in()>0);

        return pendding_size;
    }
    
};