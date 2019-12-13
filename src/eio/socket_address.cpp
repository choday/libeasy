#define _CRT_SECURE_NO_WARNINGS
#include "socket_address.hpp"

#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "../ebase/precompile.h"
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include "Ws2tcpip.h"
#define snprintf _snprintf
#else

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef  sockaddr SOCKADDR;
typedef  struct sockaddr* LPSOCKADDR;

typedef  sockaddr_in SOCKADDR_IN;
typedef  struct sockaddr_in* LPSOCKADDR_IN;

typedef  sockaddr_in6 SOCKADDR_IN6;
typedef  struct sockaddr_in6* LPSOCKADDR_IN6;
#define IN6_ADDR struct in6_addr
#endif

namespace eio
{

	socket_address::socket_address()
	{
		memset( _data,0,sizeof(_data) );
	}

	socket_address::~socket_address()
	{

	}

	bool socket_address::assign(const void* data,int len)
	{
		memcpy(_data,data,min(len,this->capacity() ));
		return true;
	}

	bool socket_address::assign(const socket_address& v)
	{
		return this->assign( v.data(),v.size() );
	}

	ebase::string socket_address::to_string(bool include_port /*=true */) const
	{
        const char* p = 0;
		ebase::string result;
		switch(family())
		{
		case AF_INET:
            p =(const char*)&LPSOCKADDR_IN(_data)->sin_addr;
			if( include_port )
			{
				result.fomart_assign( "%u.%u.%u.%u:%u", p[0], p[2], p[3], p[3],htons(LPSOCKADDR_IN( _data )->sin_port) );
			}else
			{
				result.fomart_assign( "%u.%u.%u.%u", p[0], p[2], p[3], p[3] );
			}

			break;
		case AF_INET6:

            p = ::inet_ntop( AF_INET6,&LPSOCKADDR_IN6(_data)->sin6_addr,result.resize(64,false),result.capacity() );
            if(p)result.resize((int)strlen(p));

			if(include_port) 
            {
                ebase::string temp;
                temp.fomart_assign( "[%s]:%u",result.c_str(),htons(LPSOCKADDR_IN6(_data)->sin6_port ) );
                result = temp;
            }
			break;
		default:
			assert(family()==AF_INET||family()==AF_INET6);
			return result;
		}
		return result;
	}



	socket_address& socket_address::operator=(const socket_address& v)
	{
		this->assign(v);
		return *this;
	}

	int socket_address::family() const
	{
		return LPSOCKADDR(_data)->sa_family;
	}

    int socket_address::port() const
    {
        return htons(LPSOCKADDR_IN(_data)->sin_port);
    }

    void* socket_address::data()
	{
		return _data;
	}

	const void* socket_address::data() const
	{
		return _data;
	}

    const void* socket_address::ip_data() const
    {
		switch(family())
		{
		case AF_INET:
			return &LPSOCKADDR_IN(_data)->sin_addr;
		case AF_INET6:
			return &LPSOCKADDR_IN6(_data)->sin6_addr;
		default:
			return 0;
		}
		return 0;
    }

    int socket_address::size() const
	{
		switch(family())
		{
		case AF_INET:
			return sizeof(SOCKADDR_IN);
		case AF_INET6:
			return sizeof(SOCKADDR_IN6);
		default:
			return 0;
		}
		return sizeof(_data);
	}

	int socket_address::capacity() const
	{
		return sizeof(_data);
	}
	void socket_address::set_family( bool ipv6 )
	{
		LPSOCKADDR(_data)->sa_family=ipv6?AF_INET6:AF_INET;
	}
	void socket_address::set_family(int family)
	{
		LPSOCKADDR(_data)->sa_family=family;
	}

	void socket_address::set_port(int port)
	{
		LPSOCKADDR_IN(_data)->sin_port = htons(port);
	}

	void socket_address::set_ipv6(unsigned char* ip_16bytesp)
	{
		LPSOCKADDR_IN6(_data)->sin6_family = AF_INET;
		if(ip_16bytesp)memcpy( &LPSOCKADDR_IN6(_data)->sin6_addr,ip_16bytesp,sizeof(IN6_ADDR) );
		else memset( &LPSOCKADDR_IN6(_data)->sin6_addr,0,sizeof(IN6_ADDR) );
	}

	void socket_address::set_ipv4(unsigned long ip_4bytes)
	{
		LPSOCKADDR_IN(_data)->sin_family = AF_INET;
		LPSOCKADDR_IN(_data)->sin_addr.s_addr = ip_4bytes;
	}
	
	bool socket_address::parse_ip(const char* ip)
	{
		const char* p = ip;

        if( ::inet_pton(AF_INET6,ip,&LPSOCKADDR_IN6(_data)->sin6_addr ) > 0 )
		{
			this->set_family(AF_INET6);
        }else if( ::inet_pton(AF_INET,ip,&LPSOCKADDR_IN(_data)->sin_addr ) > 0 )
		{
			this->set_family(AF_INET);
		}else
		{
			return false;
		}
		return true;
	}

	bool socket_address::parse_ip_port(const char* ip)
	{
		int size=(int)strlen(ip);
		const char* port = ip + size-1;
		for( ;port>=ip;--port )
		{
			if(':'==*port)break;
		}
		if(port==0 || port<=ip || port+2 >= ip+size )return false;//Ã»ÓÐport

		if( *(port-1) == ']' )
		{
			if(*ip!='[')return false;

			if( inet_pton(AF_INET6,ip,&LPSOCKADDR_IN(_data)->sin_addr ) > 0  )
			{
				this->set_family(AF_INET6);
				this->set_port( atoi(port+1) );
				return true;
			}

		}else
		{
			if( inet_pton(AF_INET,ip,&LPSOCKADDR_IN(_data)->sin_addr ) > 0  )
			{
				this->set_family(AF_INET);
				this->set_port( atoi(port+1) );
				return true;
			}
		}
		
		return false;
	}


};