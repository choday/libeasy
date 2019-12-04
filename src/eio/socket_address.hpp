#pragma once
#include "../ebase/string.hpp"
namespace eio
{
#define MAX_SOCKET_ADDRESS_SIZE 28
	class socket_address
	{
	public:
		socket_address();
		~socket_address();

		bool		assign(const void* data,int len);
		bool		assign( const socket_address& v);
		socket_address&	operator=(const socket_address& v);

		int			family() const;
        int         port() const;
		void*		data();
		const void*	data() const;
		int			size() const;
		int			capacity() const;


		void		set_family( bool ipv6 = false );
		void		set_family(int family);
		void		set_port(int port);
		void		set_ipv6(unsigned char* ip_16bytesp);
		void		set_ipv4(unsigned long ip_4bytes);

		bool		parse_ip(const char* ip,int size = 0);//ipv4,ipv6
		bool		parse_ip_port(const char* ip,int size = 0);

		ebase::string	to_string(bool include_port =true ) const;


		static ebase::string inet_ntop6(const unsigned char* in_ipv6_16bytes);
		static const char* inet_ntop4(const unsigned char* in_ipv4_4bytes,char *dst, size_t size);//size±ØÐë´óÓÚ16
		static ebase::string inet_ntop4(const unsigned char* in_ipv4_4bytes);
		
		static bool			inet_pton6(unsigned char* out_ipv6_16bytes,const char* ip_str,int size = 0);
		static bool			inet_pton4(unsigned char* out_ipv4_4bytes,const char* ipv4_str,int size = 0);
	private:
		char		_data[MAX_SOCKET_ADDRESS_SIZE];
	};

};
