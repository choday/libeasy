#include "string_view.hpp"
#include <string.h>
#include "string.hpp"

namespace ebase
{

    string_view::string_view(const char* p,int s ):begin(0),end(0)
    {
        assign(p,s);
    }

    string_view::string_view(const string& v):begin(0),end(0)
    {
        assign(v);
    }

    ebase::string_view& string_view::assign(const char* p,int s /*= 0*/)
    {   
        begin = p;
        end = p+s;
        return *this;
    }

    ebase::string_view& string_view::assign(const string& v)
    {
        begin = v.data();
        end = begin+v.length();
        return *this;
    }

    ebase::string_view& string_view::assign(const char* p,const char* pend)
    {
        this->begin = p;
        this->end=pend;
        return *this;
    }

    ebase::string string_view::to_string() const
    {
        return ebase::string(begin,int(end-begin));
    }

    int string_view::compare(const char* data,int size /*= 0*/) const
    {
		if(0==data)return 1;
		if(0==size)size=(int)strlen(data);

		if(this->length()>size)return 1;
		if(this->length()<size)return -1;

		return memcmp( this->begin,data,size );
    }

	int string_view::find(const char* data,int size,int pos) const
	{
		if(0==size)size=(int)strlen(data);

		const char* begin = this->begin;
		const char* end = this->end-size+1;

		for( const char* p = begin+pos;p<end;++p )
		{
			if(memcmp( p,data,size ) == 0 )return int(p-begin);
		}
		return -1;
	}

	int string_view::find_reverse(const char* data,int size,int pos) const
	{
		if(0==size)size=(int)strlen(data);

		const char* begin = this->begin;
		const char* end = this->end-size+1;

		for( const char* p = end-1;p>=begin+pos;--p )
		{
			if(memcmp( p,data,size ) == 0 )return int(p-begin);
		}
		return -1;
	}

	int string_view::find(char ch) const
	{
		char* p = (char*)memchr( this->begin,ch,this->length()+1 );
		if(0==p)return -1;
		return int(p-this->begin);
	}

	int string_view::find_reverse(char ch) const
	{
		const char* begin = this->begin;
		const char* end = this->end;

		const char* p = end;

		for( p = end-1;p>=begin;--p )
		{
			if(ch==*p)break;
		}
		return int(p-begin);
	}

};