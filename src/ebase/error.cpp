#include "error.hpp"


#ifdef _WIN32
#include <Windows.h>
#endif
#define __STDC_LIB_EXT1__ 
#include <errno.h>
#include <string.h>
namespace ebase
{

	error::error(int _code):code(_code)
	{
		if(_code)this->set_system_error(_code);
	}

    int error::last_error()
    {
#ifdef _WIN32
        return ::GetLastError();
#else
        return errno;
#endif
    }

    bool error::set_user_error(int code,const char* msg)
	{
		this->code = code;
		this->message.assign(msg);
		return true;
	}

	void error::clear()
	{
		this->code=0;
		this->message.clear();
	}

#ifdef _WIN32
	bool error::set_system_error(int code)
	{
		this->code = code;

		char*	p=0;

		DWORD chars = ::FormatMessageA(  
			FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,  
			code,  
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  
			(LPSTR) &p,  
			0, NULL ); 

		if(!chars||!p)
		{
			this->message.fomart_assign("unknown error(%d)",code);
			return false;
		}
		
		int size = (int)strlen(p);
		if( 0x0a == p[size-1] )size--;
		if( 0x0d == p[size-1] )size--;


		this->message.assign(p,size);
		::LocalFree(p);

		return true;
	}
#else
	bool error::set_system_error(int code)
	{
		this->code = code;

		this->message.assign(strerror(code));

		return true;
	}
#endif
};