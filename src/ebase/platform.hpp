#pragma once
#include <stdint.h>
#include "buffer.hpp"
#include "string.hpp"
namespace ebase
{
	namespace platform
	{


        uint64_t                get_tick_count(bool high_resolution=false);//返回自系统 启动以来的时间，单位ms(毫秒),在win32上精确度较低,high_resolution表示高高分辨率时间，最高精度为1ms

		int		                get_cpu_count();
		int		                get_last_error();

        string                  unicode_string_to_utf8(const wchar_t* wstr,int count);
        static inline string	unicode_string_to_utf8(const buffer& wstr){return unicode_string_to_utf8((const wchar_t*)wstr.data(),wstr.size()/sizeof(wchar_t));}
        buffer                  string_to_unicode_win32(const char* p,int size,bool is_utf8);

		static inline buffer    ansi_string_to_unicode(const string& value){return string_to_unicode_win32( value.data(),value.length(),false );}
		static inline buffer    utf8_string_to_unicode(const string& value){return string_to_unicode_win32( value.data(),value.length(),true );}

	};
	

};