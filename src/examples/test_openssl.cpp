#include <openssl/ssl.h>
#include <winsock2.h>
#include <openssl/err.h>
	int get_nread_size(SOCKET _handle)
	{
		u_long size = 0;
		int result = 0;
		result = ::ioctlsocket(_handle,FIONREAD,&size );
		if(result)return -1;
		return size;
	}
void test_openssl()
{
    OPENSSL_init_ssl(OPENSSL_INIT_ENGINE_ALL_BUILTIN| OPENSSL_INIT_LOAD_CONFIG, NULL);

    SSL_CTX* ctx = SSL_CTX_new(TLS_method());
        SSL *                           _ssl;
        BIO *                           _wbio;
        BIO*                            _rbio;

        _ssl = SSL_new(ctx);
        _rbio = BIO_new(BIO_s_mem());
        _wbio = BIO_new(BIO_s_mem());
        SSL_set_bio(_ssl, _rbio, _wbio);
    SSL_set_connect_state(_ssl);

		static bool ws32_inited=false;
		if(!ws32_inited)
		{
			WSADATA wsaData;
			int err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

			ws32_inited=(err==0);
		}


    SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP );

    sockaddr_in addr;

    addr.sin_family =AF_INET;
    addr.sin_port=htons(443);
    addr.sin_addr.S_un.S_addr = inet_addr("112.80.248.75");

    connect( s,(const sockaddr *)&addr,sizeof(addr) );

    char buffer[1024];
    int n=0;
    int ret = 0;
    do 
    {
        ret = SSL_do_handshake(_ssl);
        if(ret>0)break;

        int result = SSL_get_error(_ssl,ret );
        if( result == SSL_ERROR_WANT_READ )
        {
            do 
            {
                n = BIO_read( _wbio, buffer, sizeof(buffer) );

                if(n>0)
                {
                    send( s,buffer,n,0 );
                }
            } while (n>0);
        }else if( result == SSL_ERROR_WANT_WRITE )
        {
            int n = recv(s,buffer,sizeof(buffer),0);
            if(n>0)BIO_write(_rbio, buffer,n );
        }else
        {

            printf("error\n");
        }
        while(get_nread_size(s)>0) 
        {
            n = recv(s,buffer,sizeof(buffer),0);
            if(n>0)BIO_write(_rbio, buffer,n );

            ret = SSL_do_handshake(_ssl);
            if(ret>0)break;

        } 

    } while (ret<=0);

    char* p =
    "GET / HTTP/1.1\r\n"
    "Host: www.baidu.com\r\n"
    "Accept-Encoding: gzip\r\n"
    "Connection: keep-alive\r\n"
    "\r\n";

    ret = SSL_write( _ssl,p,strlen(p) );
            do 
            {
                n = BIO_read( _wbio, buffer, sizeof(buffer) );

                if(n>0)
                {
                    send( s,buffer,n,0 );
                }
            } while (n>0);
            Sleep(1000);
        while(get_nread_size(s)>0) 
        {
            n = recv(s,buffer,sizeof(buffer),0);
            if(n>0)ret=BIO_write(_rbio, buffer,n );

            n = BIO_flush(_rbio);
            Sleep(100);
            if(SSL_pending(_ssl)>0)
            {

                n = SSL_read( _ssl,buffer,sizeof(buffer) );
                if(n>0)
                {
                    buffer[n] = 0;
                    printf(buffer);
                }
            }

        } 
        n=SSL_pending(_ssl);
        n=SSL_get_read_ahead(_ssl);
        n = SSL_read( _ssl,buffer,sizeof(buffer)-1 );

        if(n>0)
        {
            buffer[n] =0;
            printf("%s\n",buffer);

        }

            do 
            {
                n = BIO_read( _wbio, buffer, sizeof(buffer) );

                if(n>0)
                {
                    send( s,buffer,n,0 );
                }
            } while (n>0);
};