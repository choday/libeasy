#pragma once
#include "ref_memory.hpp"

namespace ebase
{
	struct ref_string
	{

		struct header:public ref_memory::header
		{
			char			data[sizeof(void*)];
		};

        static const int        header_size;

		void					init(struct header* p=0);
		void					clear();

		const char*				data() const;
		const char*				c_str() const;
		int						size() const;
		int						capacity() const;

		//如果size=0,则复制按最多能复制的字节进行计算
		bool					assign(const ref_string& v,int offset=0,int size=0);//[must]offset>=0,size>=0
		bool					assign(const char* data,int size = 0);
		bool					append(const char* data,int size = 0);
		//返回可写缓冲区
		char*					resize(int size,bool keep_data = true);

		header*					get_header() const;//返回的header,ref_count+1,可能返回0
		
		int						_offset;
		int						_size;
        ref_memory_t<header>	_data;
	};
};