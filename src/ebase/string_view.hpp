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
        string_view&    assign(const char* p,const char* pend);
        string_view&    assign(const string& v);
        string_view&    operator= (const string& v) {return this->assign(v);}
        string_view&    operator= (const char* p) {return this->assign(p);}

		int			find(const char* data,int size,int pos) const;
		int			find_reverse(const char* data,int size,int pos) const;
		int			find(char ch) const;
		int			find_reverse(char ch) const;

        inline const char*      data() const {return this->begin;}
        inline int              length() const {return int(this->end-this->begin);}
        inline void             clear(){ begin=0;end=0;}
        string                  to_string() const;

        int			        compare(const char* data,int size = 0) const;
        inline int			compare(const string_view& v) const{return this->compare( v.data(),v.length() );}

		//±È½Ï·ûºÅ
#define DEF_STRING_COMPARE_OPERATE(METHOD) inline bool	operator METHOD(const char* data) const{return this->compare(data) METHOD 0;}; inline bool	operator METHOD(const string_view& v) const{return this->compare(v) METHOD 0;};
	DEF_STRING_COMPARE_OPERATE(>=);
	DEF_STRING_COMPARE_OPERATE(>);
	DEF_STRING_COMPARE_OPERATE(<=);
	DEF_STRING_COMPARE_OPERATE(<);
	DEF_STRING_COMPARE_OPERATE(==);
	DEF_STRING_COMPARE_OPERATE(!=);
#undef DEF_STRING_COMPARE_OPERATE

        const char*     begin;
        const char*     end;
    };

};