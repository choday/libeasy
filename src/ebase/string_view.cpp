#include "string_view.hpp"
#include <string.h>
#include "string.hpp"

namespace ebase
{

    string_view::string_view(const char* p,int s ):_data(0),_size(0)
    {
        assign(p,s);
    }

    string_view::string_view(const string& v):_data(0),_size(0)
    {
        assign(v);
    }

    ebase::string_view& string_view::assign(const char* p,int s /*= 0*/)
    {   
        if(0==s && p)s=(int)strlen(p);

        _data = p;
        _size = s;
        return *this;
    }

    ebase::string_view& string_view::assign(const string& v)
    {
        _data = v.data();
        _size = v.length();
        return *this;
    }

    ebase::string string_view::to_string() const
    {
        return ebase::string(_data,_size);
    }

};