#pragma once
#include "string.hpp"
namespace ebase
{

	class error
	{
	public:
		error(int code=0);

        static int      last_error();

		int				code;
		ebase::string	message;

		bool			set_system_error(int code );
		bool			set_user_error( int code,const char* msg );
		void			clear();
	};
};