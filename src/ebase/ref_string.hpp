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

		//���size=0,���ư�����ܸ��Ƶ��ֽڽ��м���
		bool					assign(const ref_string& v,int offset=0,int size=0);//[must]offset>=0,size>=0
		bool					assign(const char* data,int size = 0);
		bool					append(const char* data,int size = 0);
		//���ؿ�д������
		char*					resize(int size,bool keep_data = true);

		header*					get_header() const;//���ص�header,ref_count+1,���ܷ���0
		
		int						_offset;
		int						_size;
        ref_memory_t<header>	_data;
	};
};