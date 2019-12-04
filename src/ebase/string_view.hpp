#pragma once

namespace ebase
{
    class string;

    class string_view
    {
    public:
        string_view(const char* p=0,int size = 0);
        string_view(const string& v);

        string_view&    assign(const char* p,int size = 0);
        string_view&    assign(const string& v);
        string_view&    operator= (const string& v) {return this->assign(v);}
        string_view&    operator= (const char* p) {return this->assign(p);}

        inline const char*      data() const {return this->_data;}
        inline int              length() const {return this->_size;}

        string                  to_string() const;

    private:
        const char*     _data;
        int             _size;
    };

};