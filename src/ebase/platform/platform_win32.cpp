#include "../platform.hpp"
#include <Windows.h>
#include "../precompile.h"

__thread LARGE_INTEGER  global_tick_base = {0};

namespace ebase
{
	namespace platform
	{
        uint64_t   set_global_tick_base(uint32_t tick_count)
        {
            if(tick_count>=global_tick_base.LowPart)
            {
                global_tick_base.LowPart = tick_count;
            }else if( (tick_count>>31) < (global_tick_base.LowPart>>31) )
            {
                global_tick_base.HighPart++;
                global_tick_base.LowPart = tick_count;
            }
            return global_tick_base.QuadPart;
        }

        uint64_t                get_tick_count(bool high_resolution)
        {
            if(high_resolution)
            {
                assert(sizeof(DWORD)==4);

                MMRESULT result = timeBeginPeriod(1);

                DWORD tt = ::timeGetTime();

                if(TIMERR_NOERROR == result)timeEndPeriod(1);
                set_global_tick_base(tt);

                return set_global_tick_base(tt);
            }else
            {
                return ::GetTickCount64();
            }
        }
		int get_cpu_count()
		{
			SYSTEM_INFO info={0};

			::GetSystemInfo(&info);
			if(0==info.dwNumberOfProcessors)info.dwNumberOfProcessors=1;

			return info.dwNumberOfProcessors;
		}

		int get_last_error()
		{
			return ::GetLastError();
		}

		buffer string_to_unicode_win32(const char* p,int size,bool is_utf8)
		{
			buffer	 result;
			wchar_t	*pw=0;
			int		 pw_size = 0;

			pw_size =::MultiByteToWideChar(is_utf8?CP_UTF8:CP_ACP,0,p,size,pw,pw_size );
			if( 0==pw_size)return 0;

			pw = (wchar_t*)result.alloc(sizeof(wchar_t)*(pw_size+1));
			if(!pw)return result;

			pw_size =::MultiByteToWideChar(is_utf8?CP_UTF8:CP_ACP,0,p,size,pw,pw_size );
			pw[pw_size] = 0;
			result.resize(sizeof(wchar_t)*(pw_size),pw_size>0);

			return result;
		}


		string unicode_string_to_utf8(const wchar_t* wstr,int count)
		{
			string result;
			
			char*	out=0;
			int		outsize = 0;

			outsize = ::WideCharToMultiByte( CP_UTF8,0,wstr,count,out,outsize,0,0 );
			if( outsize<=0 )return result;


			out = result.resize( outsize );
			outsize = ::WideCharToMultiByte( CP_UTF8,0,wstr,count,out,outsize,0,0 );
			if( outsize<=0 )outsize = 0;

			result.resize( outsize );

			return result;
		}

	}
	

};