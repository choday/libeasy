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
        unsigned char* p = 0;
		ebase::string result;
		switch(family())
		{
		case AF_INET:
            p =(unsigned char*)&LPSOCKADDR_IN(_data)->sin_addr;
			if( include_port )
			{
				result.fomart_assign( "%u.%u.%u.%u:%u", p[0], p[2], p[3], p[3],htons(LPSOCKADDR_IN( _data )->sin_port) );
			}else
			{
				result.fomart_assign( "%u.%u.%u.%u", p[0], p[2], p[3], p[3] );
			}

			break;
		case AF_INET6:

            //p = inet_ntop( AF_INET6,&LPSOCKADDR_IN6(_data)->sin6_addr,result.resize(,false),result.capacity() );
            //if(p)result.resize(strlen(p));

			result = socket_address::inet_ntop6( (const unsigned char *)&LPSOCKADDR_IN6(_data)->sin6_addr);
			if(include_port) result += ebase::string().fomart_assign(":%u",htons(LPSOCKADDR_IN6(_data)->sin6_port ));
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

	
	bool socket_address::parse_ip(const char* ip,int size /*= 0*/)
	{
		const char* p = ip;
		if(0==size)size=(int)strlen(p);

		if( socket_address::inet_pton6( (unsigned char *)&LPSOCKADDR_IN6(_data)->sin6_addr,ip,size ) )
		{
			this->set_family(AF_INET6);
		}else if(socket_address::inet_pton4( (unsigned char *)&LPSOCKADDR_IN(_data)->sin_addr,ip,size ))
		{
			this->set_family(AF_INET);
		}else
		{
			return false;
		}
		return true;
	}

	bool socket_address::parse_ip_port(const char* ip,int size /*= 0*/)
	{
		if(0==size)size=(int)strlen(ip);
		const char* port = ip + size-1;
		for( ;port>=ip;--port )
		{
			if(':'==*port)break;
		}
		if(port==0 || port<=ip || port+2 >= ip+size )return false;//Ã»ÓÐport

		if( *(port-1) == ']' )
		{
			if(*ip!='[')return false;

			if( socket_address::inet_pton6( (unsigned char *)&LPSOCKADDR_IN(_data)->sin_addr,ip+1,port-ip-2 ) )
			{
				this->set_family(AF_INET6);
				this->set_port( atoi(port+1) );
				return true;
			}

		}else
		{
			if( socket_address::inet_pton4( (unsigned char *)&LPSOCKADDR_IN(_data)->sin_addr,ip,port-ip ) )
			{
				this->set_family(AF_INET);
				this->set_port( atoi(port+1) );
				return true;
			}
		}
		

		return false;
	}

#ifndef IN6ADDRSZ
#define IN6ADDRSZ   16
#endif

#ifndef INT16SZ
#define INT16SZ 2
#endif

	ebase::string socket_address::inet_ntop6(const unsigned char* ipv6_16bytes)
	{
		const unsigned char* src=ipv6_16bytes;

		/*
		 * Note that int32_t and int16_t need only be "at least" large enough
		 * to contain a value of the specified size.  On some systems, like
		 * Crays, there is no such thing as an integer variable with 16 bits.
		 * Keep this in mind if you think this function should have been coded
		 * to use pointer overlays.  All the world's not a VAX.
		 */
		char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
		struct { int base, len; } best = {-1, 0}, cur = {-1, 0};
		unsigned int words[IN6ADDRSZ / INT16SZ];
		int i;
		const unsigned char *next_src, *src_end;
		unsigned int *next_dest;

		/*
		 * Preprocess:
		 *	Copy the input (bytewise) array into a wordwise array.
		 *	Find the longest run of 0x00's in src[] for :: shorthanding.
		 */
		next_src = src;
		src_end = src + IN6ADDRSZ;
		next_dest = words;
		i = 0;
		do {
			unsigned int next_word = (unsigned int)*next_src++;
			next_word <<= 8;
			next_word |= (unsigned int)*next_src++;
			*next_dest++ = next_word;

			if (next_word == 0) 
			{
				if (cur.base == -1) 
				{
					cur.base = i;
					cur.len = 1;
				}
				else 
				{
					cur.len++;
				}
			} else 
			{
				if (cur.base != -1) 
				{
					if (best.base == -1 || cur.len > best.len) 
					{
						best = cur;
					}
					cur.base = -1;
				}
			}

			i++;
		} while (next_src < src_end);

		if (cur.base != -1) 
		{
			if (best.base == -1 || cur.len > best.len) 
			{
				best = cur;
			}
		}
		if (best.base != -1 && best.len < 2) 
		{
			best.base = -1;
		}

		/*
		 * Format the result.
		 */
		tp = tmp;
		for (i = 0; i < (IN6ADDRSZ / INT16SZ);) 
		{
			/* Are we inside the best run of 0x00's? */
			if (i == best.base) 
			{
				*tp++ = ':';
				i += best.len;
				continue;
			}
			/* Are we following an initial run of 0x00s or any real hex? */
			if (i != 0) 
			{
				*tp++ = ':';
			}
			/* Is this address an encapsulated IPv4? */
			if (i == 6 && best.base == 0 &&
				(best.len == 6 || (best.len == 5 && words[5] == 0xffff))) 
			{
				if (!socket_address::inet_ntop4(src+12, tp, sizeof tmp - (tp - tmp))) 
				{
					return (NULL);
				}
				tp += strlen(tp);
				break;
			}
			tp += snprintf(tp, sizeof tmp - (tp - tmp), "%x", words[i]);
			i++;
		}
		/* Was it a trailing run of 0x00's? */
		if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ)) 
		{
			*tp++ = ':';
		}
		*tp++ = '\0';

		return ebase::string( tmp );
	}

	const char* socket_address::inet_ntop4(const unsigned char* ipv4_4bytes,char *dst, size_t size)
	{
		const unsigned char *src = ipv4_4bytes;
		const size_t MIN_SIZE = 16; /* space for 255.255.255.255\0 */
		int n = 0;
		char *next = dst;

		if (size < MIN_SIZE) 
		{
			return NULL;
		}
		do 
		{
			unsigned char u = *src++;
			if (u > 99) {
			*next++ = '0' + u/100;
			u %= 100;
			*next++ = '0' + u/10;
			u %= 10;
			}
			else if (u > 9) {
			*next++ = '0' + u/10;
			u %= 10;
			}
			*next++ = '0' + u;
			*next++ = '.';
			n++;
		} while (n < 4);
		*--next = 0;
		return dst;
	}

	ebase::string socket_address::inet_ntop4(const unsigned char* ipv4_4bytes)
	{
		ebase::string result;

		const char* p = socket_address::inet_ntop4( ipv4_4bytes,result.resize( 30,false ),30 );
		if(p)result.resize( strlen(p),true);
		else result.clear();
		return result;
	}
#ifndef IN6ADDRSZ
#define IN6ADDRSZ   16
#endif

#ifndef INT16SZ
#define INT16SZ 2
#endif

#ifndef INADDRSZ
#define INADDRSZ    4
#endif
	bool socket_address::inet_pton6(unsigned char* ipv6_16bytes,const char* ip_str,int size /*= 0*/)
	{
		const char*			src = ip_str;
		if(0==size)size=(int)strlen(src);

		static const char	xdigits_l[] = "0123456789abcdef",
							xdigits_u[] = "0123456789ABCDEF";
		unsigned char		tmp[IN6ADDRSZ], *tp, *endp, *colonp;
		const char			*xdigits, *curtok;
		int					ch, saw_xdigit;
		unsigned int		val;

		memset((tp = tmp), '\0', IN6ADDRSZ);
		endp = tp + IN6ADDRSZ;
		colonp = NULL;
		/* Leading :: requires some special handling. */
		if (*src == ':')
			if (*++src != ':')
				return false;

		curtok = src;
		saw_xdigit = 0;
		val = 0;
		while ((src-ip_str<size) && (ch = *src++) != '\0' ) 
		{
			const char *pch;

			if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
				pch = strchr((xdigits = xdigits_u), ch);

			if (pch != NULL) 
			{
				val <<= 4;
				val |= (pch - xdigits);
				if (val > 0xffff)
					return (false);
				saw_xdigit = 1;
				continue;
			}
			if (ch == ':') 
			{
				curtok = src;
				if (!saw_xdigit) 
				{
					if (colonp)	return (false);

					colonp = tp;
					continue;
				}

				if (tp + INT16SZ > endp)return (false);

				*tp++ = (unsigned char) (val >> 8) & 0xff;
				*tp++ = (unsigned char) val & 0xff;
				saw_xdigit = 0;
				val = 0;
				continue;
			}
			if (ch == '.' && ((tp + INADDRSZ) <= endp) &&
				socket_address::inet_pton4(tp,curtok,size-(curtok-ip_str) )) 
			{
				tp += INADDRSZ;
				saw_xdigit = 0;
				break;	/* '\0' was seen by inet_pton4(). */
			}
			return (false);
		}
		if (saw_xdigit) 
		{
			if (tp + INT16SZ > endp)return (0);

			*tp++ = (unsigned char) (val >> 8) & 0xff;
			*tp++ = (unsigned char) val & 0xff;
		}
		if (colonp != NULL) 
		{
			/*
			 * Since some memmove()'s erroneously fail to handle
			 * overlapping regions, we'll do the shift by hand.
			 */
			const int n = tp - colonp;
			int i;

			for (i = 1; i <= n; i++) 
			{
				endp[- i] = colonp[n - i];
				colonp[n - i] = 0;
			}
			tp = endp;
		}
		if (tp != endp)return (false);

		memcpy(ipv6_16bytes, tmp, IN6ADDRSZ);
		return (true);
	}

	bool socket_address::inet_pton4(unsigned char* ipv4_4bytes,const char* ipv4_str,int size /*= 0*/)
	{
		const char*			src = ipv4_str;
		if(0==size)size=(int)strlen(src);

		static const char	digits[] = "0123456789";
		int					saw_digit, octets, ch;
		unsigned char		tmp[INADDRSZ], *tp;
		unsigned int		newval;

		saw_digit	= 0;
		octets		= 0;
		*(tp = tmp) = 0;

		while ((src-ipv4_str<size)  && (ch = *src++) != '\0' ) 
		{
			const char *pch;

			if ((pch = strchr(digits, ch)) != NULL) 
			{
				newval = *tp * 10 + (unsigned int)(pch - digits);

				if (newval > 255)return false;

				*tp = newval;
				if (! saw_digit) 
				{
					if (++octets > 4)return false;
					saw_digit = 1;
				}
			} else if (ch == '.' && saw_digit) 
			{
				if (octets == 4)return false;
				*++tp = 0;
				saw_digit = 0;
			} else
				return false;
		}

		if (octets < 4)return false;

		memcpy(ipv4_4bytes, tmp, INADDRSZ);
		return true;
	}

};