#pragma once
#include "../ebase/string.hpp"
#include "base/io_request.hpp"

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netdb.h>   
#include <sys/socket.h> 
#include <errno.h>
#endif

namespace eio
{
	class name_resolver
	{
	public:

		//name����"www.baidu.com","127.0.0.1"������ʽ�����룬service_of_port����"80","https"����������
		static bool resolver(const ebase::string& name,const ebase::string& service_of_port,const io_callback& callback,ebase::executor* callback_executor);

		class request:public io_request
		{
		public:
			request();
			~request();


			ebase::string		host;
			ebase::string		port;

            bool                is_success();
            const void*         move_begin();
            const void*         move_next();

            const void*         current();
            const int           current_size();

			void				clear();
		private:
			virtual void		internal_do_request();

			void* const     	result ;//�������û��޸�
			void*	            _current;//�ṩһ�����������û�ʹ��

		};

		static ebase::ref_ptr<request> create_request();
	};


};