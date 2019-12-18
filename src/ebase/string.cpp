#define _CRT_SECURE_NO_WARNINGS
#include "string.hpp"
#include "precompile.h"
#include "allocator.hpp"
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include<ctype.h>
#ifndef _WIN32
inline char* _strlwr(char* str)
{
    char* orig = str;
    // process the string
    for (; *str != 0; str++)
        *str = tolower(*str);
    return orig;
}
inline char* _strupr(char* str)
{
    char* orig = str;
    // process the string
    for (; *str != 0; str++)
        *str = toupper(*str);
    return orig;
}
#endif
namespace ebase
{
	void string::call_constructor()
	{
		_private_data.size = 0;
		_private_data.local_data[0] = 0;
	}

	
	void string::call_constructor(const string& v)
	{
		_private_data.size = 0;
		_private_data.local_data[0] = 0;

		this->assign( v );
	}

	string::string()
	{
		_private_data.size = 0;
		_private_data.local_data[0] = 0;
	}

	string::string(const char* data,int size /*= 0*/)
	{
		_private_data.size = 0;
		_private_data.local_data[0] = 0;

		this->assign( data,size );
	}

	string::string(const string& v,int offset/*=0*/,int size/*=0*/)
	{
		_private_data.size = 0;
		_private_data.local_data[0] = 0;

		this->assign( v,offset,size );

	}

	string::~string()
	{
		clear();
	}

	void string::clear()
	{
		if(_private_data.size==private_data_type_for_ref)
		{
			_private_data.ref_data.clear();
		}

		_private_data.size = 0;
		_private_data.local_data[0] = 0;
	}

	char* string::resize(int size,bool keep_data /*= true*/)
	{
		ref_string	hold_ref;

		if(0==size)
		{
			this->clear();
			return this->_private_data.local_data;
		}else if(_private_data.type_for_ref == private_data_type_for_ref)
		{
			if(size>max_local_data_capacity)
			{
				return _private_data.ref_data.resize( size,keep_data );
			}else 
			{
				if(keep_data)
				{
					hold_ref.init();
					hold_ref.assign( _private_data.ref_data,0,size );
				}

				_private_data.ref_data.clear();

				if(keep_data)
				{
					memcpy( _private_data.local_data,hold_ref.data(),size );
					hold_ref.clear();
				}

				_private_data.size_for_local_data = size;
				_private_data.local_data[size] = 0;
				return _private_data.local_data;
			}
		}else
		{
			if(size>max_local_data_capacity)
			{
				hold_ref.init();
				char* p =hold_ref.resize( size,false );

				if(keep_data&&_private_data.size_for_local_data)memcpy(p,_private_data.local_data,_private_data.size_for_local_data);

				_private_data.type_for_ref = private_data_type_for_ref;
				_private_data.ref_data.init();
				_private_data.ref_data.assign( hold_ref );
				
				hold_ref.clear();

				return p;
			}else
			{
				_private_data.size_for_local_data = size;
				_private_data.local_data[size] = 0;
				return _private_data.local_data;
			}
		}
	}

	int string::capacity() const
	{
		if(_private_data.size==private_data_type_for_ref)return _private_data.ref_data.capacity();
		return max_local_data_capacity;
	}

	int string::size() const
	{
		if(_private_data.size==private_data_type_for_ref)return _private_data.ref_data.size();
		return _private_data.size;
	}

	const char* string::data() const
	{
		if(_private_data.size==private_data_type_for_ref)return _private_data.ref_data.data();
		return _private_data.local_data;
	}

	const char* string::c_str() const
	{
		if(_private_data.size==private_data_type_for_ref)return _private_data.ref_data.c_str();
		return _private_data.local_data;
	}

	string& string::fomart_assign(const char* formatstring,...)
	{
		va_list args;
		va_start(args, formatstring);

        int capacity = max_local_data_capacity;

		char* p = this->resize(capacity,false);

		int size = vsnprintf( p,capacity+1,formatstring, args); // C4996

		while(-1==size)
		{
			capacity += 32;
			p = this->resize(capacity,false);
			size = vsnprintf( p,capacity+1,formatstring, args); // C4996
		}
		this->resize(size,true);
		va_end(args);

		return *this;
	}

	string& string::assign(const char* data,int size /*= 0*/)
	{
		if(0==size && data)size=(int)strlen(data);
		if(data)
		{
			char* p = this->resize(size,false);
			memcpy( p,data,size );
		}else
		{
			clear();
		}		
		return *this;
	}

	string& string::assign(const string& v,int offset/*=0*/,int size/*=0*/)
	{
		if(offset<0)offset=v.size()+offset;
		if(offset<0 || offset>= v.size() )
		{
			this->clear();return *this;
		}

		if(size<=0)size=(v.size()-offset)+size;
		size = min(size,v.size()-offset);

		if(size<=0)
		{
			this->clear();return *this;
		}

		if(this==&v)
		{
			if(_private_data.type_for_ref == private_data_type_for_ref)
			{
				_private_data.ref_data.assign( v._private_data.ref_data,offset,size );
			}else if(offset>0 )
			{
				memmove(&_private_data.local_data,v.data()+offset,size );
				_private_data.size_for_local_data = size;
				_private_data.local_data[size] = 0;
			}
			return *this;
		}

		this->clear();

		if(size<=max_local_data_capacity)
		{

			_private_data.size_for_local_data = size;
			memmove(&_private_data.local_data,v.data()+offset,size );
			_private_data.local_data[size]=0;

		}else if(v._private_data.type_for_ref == private_data_type_for_ref )//vµÄsize()>max_local_data_capacity,±ØÐëÎªprivate_data_type_for_ref
		{
			//assert(v._private_data.type_for_ref == private_data_type_for_ref );

			_private_data.type_for_ref=private_data_type_for_ref;
			_private_data.ref_data.init();
			_private_data.ref_data.assign( v._private_data.ref_data,offset,size );
		}else
		{
			assert(v._private_data.type_for_ref == private_data_type_for_ref );
		}

		return *this;
	}



	string& string::append(const char* data,int size /*= 0*/)
	{
		if(0==size && data)size=(int)strlen(data);
		if(data&&size>0)
		{
			if(_private_data.type_for_ref == private_data_type_for_ref)
			{
				_private_data.ref_data.append( data,size );
			}else
			{
				int origin_size=this->size();

				memcpy( this->resize(size+origin_size,true)+origin_size,data,size );
			}
		}
		return *this;
	}


	int string::compare(const char* data,int size /*= 0*/) const
	{
		if(0==data)return 1;
		if(0==size)size=(int)strlen(data);

		if(this->size()>size)return 1;
		if(this->size()<size)return -1;

		return memcmp( this->data(),data,size );
	}

    int string::compare_ignore_case(const char* data,int size /*= 0*/) const
    {
		if(0==data)return 1;
		if(0==size)size=(int)strlen(data);

		if(this->size()>size)return 1;
		if(this->size()<size)return -1;

        return strncmp( this->data(),data,size );
    }

    int string::compare_size(const char* data,int compare_chars) const
    {
        if(0==data)return 1;
        if(this->size()<compare_chars)return -1;
        return strncmp( this->data(),data,compare_chars );
    }

    int string::compare_size_ignore_case(const char* data,int compare_chars) const
    {
        if(0==data)return 1;
        if(this->size()<compare_chars)return -1;
        return strnicmp( this->data(),data,compare_chars );
    }

    string& string::make_upper()
	{
		int size=this->size();
		char*	p=this->resize(size,true);
		_strupr(p);
		return *this;
	}

    

	string& string::make_lower()
	{
		int size=this->size();
		char*	p=this->resize(size,true);
		_strlwr(p);
		return *this;
	}

	int string::find(const char* data,int size,int pos) const
	{
		if(0==size)size=(int)strlen(data);

		const char* begin = this->data();
		const char* end = begin+this->size()-size+1;

		for( const char* p = begin+pos;p<end;++p )
		{
			if(memcmp( p,data,size ) == 0 )return int(p-begin);
		}
		return -1;
	}

	int string::find_reverse(const char* data,int size,int pos) const
	{
		if(0==size)size=(int)strlen(data);

		const char* begin = this->data();
		const char* end = begin+this->size()-size+1;

		for( const char* p = end-1;p>=begin+pos;--p )
		{
			if(memcmp( p,data,size ) == 0 )return int(p-begin);
		}
		return -1;
	}

	int string::find(char ch) const
	{
		const char* src = this->data();
		char* p = (char*)memchr( this->data(),ch,this->size()+1 );
		if(0==p)return -1;
		return int(p-src);
	}

	int string::find_reverse(char ch) const
	{
		const char* begin = this->data();
		const char* end = begin+this->size();

		const char* p = end;

		for( p = end-1;p>=begin;--p )
		{
			if(ch==*p)break;
		}
		return int(p-begin);
	}

	string string::replace(const char* find_string,int find_size,const char* replace_string,int replace_size /*= 0*/) const
	{
		if(0==find_size)find_size=(int)strlen(find_string);
		if(0==replace_size)find_size=(int)strlen(replace_string);

		if(find_string==replace_string && find_size == replace_size )return *this;

		string result;

		int start_pos = 0;
		const char* p =this->data();

		int ret=-1;

		do 
		{
			ret = this->find( find_string,find_size,start_pos );
			if( ret == -1 )
			{
				result.append( p+start_pos,this->size()-start_pos );
				break;
			}

			if(ret-start_pos>0)result.append(p+start_pos,ret-start_pos);
			result.append(replace_string,replace_size);

			start_pos=ret+find_size;

		} while (ret>-1);


		return result;
	}

	string string::substr(const string& left,const string& right,int start_post,bool return_left/*=false*/,bool return_right/*=false*/)
	{
		int left_pos = this->find( left,start_post );
		if( left_pos == -1 )return string();

		int right_pos = this->find(right,left_pos+left.size() );
		if( right_pos == -1 )return string();

		if(!return_left)left_pos += left.size();

		if(return_right)right_pos+=right.size();

		if(right_pos==left_pos)return string();

		return string( *this,left_pos,right_pos-left_pos);
	}



	string_array string::split(const string& delimiter,int limit)
	{
		string_array result;

		int start_pos = 0;

		int end_pos;

		do 
		{
			end_pos = this->find( delimiter,start_pos );
			if(end_pos!=-1)
			{
				if(end_pos>start_pos)result.push( this->substr( start_pos,end_pos-start_pos) );
				else result.push(string());

				start_pos = end_pos+delimiter.length();
			}else if( start_pos <= this->length() )
			{
				result.push( this->substr( start_pos,this->length()-start_pos) );
			}

		} while (end_pos!=-1&&result.size()<limit);
		
		return result;
	}



	string_array::string_array():_data(0)
	{

	}

	string_array::string_array(const string_array& v):_data(0)
	{
		this->assign(v);
	}

	string_array::~string_array()
	{
		clear();
	}

	void string_array::clear()
	{
		if(_data)_data->release();
		_data = 0;
	}

	string_array& string_array::assign(const string_array& v)
	{

		if(_data)_data->release();
		_data = v._data;
		if(_data)_data->add_ref();

		return *this;
	}

	string_array& string_array::operator=(const string_array& v)
	{
		return this->assign(v);
	}

	string_array& string_array::push(const string& data)
	{

		string* p = this->resize( this->size()+1 );

		p[this->size()-1].assign(data);

		return *this;
	}

	string string_array::pop()
	{
		if(!_data || !_data->count )return string();

		string result( _data->data[this->size()-1] );

		this->resize( this->size()-1 );

		return result;
	}

	const string& string_array::at(int index) const
	{
		assert( index < this->size() );
		if(!_data || !_data->count )return _dummy;
		if( index >= this->size() )return _dummy;

		return _data->data[index];
	}

	string& string_array::edit(int index)
	{
		assert(index>=0);
		
		return this->resize( index+1)[index];
	}

	int string_array::capacity() const
	{
		if(!_data)return 0;
		return _data->capacity;
	}

	int string_array::size() const
	{
		if(!_data)return 0;
		return _data->count;
	}

	string string_array::to_string(const string& seperate) const
	{
		string result;
		int total = this->size();

		for(int i =0;i<total;++i )
		{
			result.append( this->at(i) );
			if(seperate.size() && i<total-1)result.append( seperate );
		}
		return result;
	}

	string* string_array::resize(int count,bool keep_data/*=true*/)
	{
		int capacity = ((count>>2) << 2) + 4;

		if(0==_data)
		{
			_data = (header*)header::alloc( capacity*sizeof(string)+extern_header_size );

			_data->capacity = capacity;
			_data->count = count;

			for( int i =0;i<count;++i )
			{
				_data->data[i].call_constructor();
			}
			return _data->data;
		}


		long result = _data->add_ref();
		_data->release();

		if( result == 2 )
		{
			if( count > _data->capacity )
			{
				_data = (header*)_data->realloc( capacity*sizeof(string)+extern_header_size );

				for( int i = _data->count;i<count;++i )
				{
					_data->data[i].call_constructor();
				}

				_data->capacity = capacity;
				_data->count = count;
			}else if( count >= _data->count )
			{
				for( int i = _data->count;i<count;++i )
				{
					_data->data[i].call_constructor();
				}
				_data->count = count;
			}else
			{
				for( int i = count;i<_data->count;++i )
				{
					_data->data[i].clear();
				}
				_data->count = count;
			}
			
		}else
		{
			header* p = (header*)header::alloc( capacity*sizeof(string)+extern_header_size );

			p->capacity = capacity;
			p->count = count;

			for( int i =0;i<count;++i )
			{
				if(keep_data && i<_data->count)
				{
					p->data[i].call_constructor(_data->data[i]);
				}else
				{
					p->data[i].call_constructor();
				}
			}

			_data->release();
			_data = p;
			
		}
		return _data->data;
	}

	long string_array::header::release()
	{
		long result = ref_memory::header::release(false);
		if(0==result)
		{
			for(int i = 0;i<this->count;++i)
			{
				this->data[i].clear();
			}
			allocator::free(this);
		}
		return result;
	}

}