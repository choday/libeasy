#pragma once
#include "ref_string.hpp"
#include <string.h>
namespace ebase
{


	class string_array;

	class string
	{
	public:
		string();
		~string();
		string(const char* data,int size = 0);
		string(const string& v,int offset=0,int size=0);

		//===========>>>>�������� begin
		char*		resize(int size,bool keep_data = true);
		void		clear();
		
		int			capacity() const;
		int			size() const;
		const char* data() const;//��һ����0����
		const char* c_str() const;//��0����

		string&		fomart_assign( const char* formatstring,...);
		string&		assign( const char* data,int size = 0);
		/*
		offset����⣬offset>=0��ʱ���൱��ȥ�����offset���ַ�;offset<0��ʱ���൱��ȥ���ұ�abs(offset)���ַ�
		size����⣬ִ����һ���Ժ�size>0 �൱�ڱ���size���ַ���������ȥ��;;size<=0��ʱ���൱��ȥ��abs(size)���ַ��������ı���
		*/
		string&		assign( const string& v,int offset=0,int size=0);
		string&		append( const char* data,int size = 0);
		
		int			compare(const char* data,int size = 0) const;
		int			compare_ignore_case(const char* data,int size = 0) const;
		
        int			compare_size(const char* data,int compare_chars ) const;
        int			compare_size_ignore_case(const char* data,int compare_chars ) const;

		string&		make_upper();
		string&		make_lower();

		int			find(const char* data,int size,int pos) const;
		int			find_reverse(const char* data,int size,int pos) const;
		int			find(char ch) const;
		int			find_reverse(char ch) const;
		
		string		replace(const char* find_string,int find_size,const char* replace_string,int replace_size) const;

		string		substr( const string& left,const string& right,int start_post=0,bool return_left=false,bool return_right=false);//����left��right֮ǰ���ַ���

		
		string_array		split(const string& delimiter,int limit = 1024 );


		inline int			find(const string& v,int pos=0)const{return find(v.data(),v.size(),pos);}
		inline int			find_reverse(const string& v,int pos=0)const{return find_reverse(v.data(),v.size(),pos);}
		inline int			find(const char* data,int pos=0)const{return find(data,(int)strlen(data),pos);}
		inline int			find_reverse(const char* data,int pos=0)const{return find_reverse(data,(int)strlen(data),pos);}
		
		inline string		replace(const string& find_string,const string& replace_string )const {return replace( find_string.data(),find_string.size(),replace_string.data(),replace_string.size() );}

		inline string		to_upper_case() const{ string result(*this);return result.make_upper();}
		inline string		to_lower_case() const {string result(*this);return result.make_lower();}
		inline string		substr(int offset,int size) const {return string(*this,offset,size );}
		inline string&		append( const string& v){return this->append( v.data(),v.size() );}
		inline string&		append( char data ){ return this->append( &data,1 );}
		inline int			compare(const string& v) const{return this->compare( v.data(),v.size() );}
		inline int			length() const {return this->size();}
		//===========>>>>��������  end

		inline operator const char*() {return this->c_str();}

		//��ֵ�����
		inline string& operator =(const char* data){return this->assign( data );};
		inline string& operator =(const string& v){return this->assign( v );};
		inline string  operator +(const char* data) const {string result(*this);result.append(data);return result;}
		inline string  operator +(const string& v) const {string result(*this);result.append(v);return result;}
		inline string  operator +(char v) const {string result(*this);result.append(v);return result;}
		inline string& operator +=(const char* data){return this->append( data );}
		inline string& operator +=(const string& v){return this->append( v );}
		inline string& operator +=( char v){return this->append( v );}

		//�ȽϷ���
#define DEF_STRING_COMPARE_OPERATE(METHOD) inline bool	operator METHOD(const char* data) const{return this->compare(data) METHOD 0;}; inline bool	operator METHOD(const string& v) const{return this->compare(v) METHOD 0;};
	DEF_STRING_COMPARE_OPERATE(>=);
	DEF_STRING_COMPARE_OPERATE(>);
	DEF_STRING_COMPARE_OPERATE(<=);
	DEF_STRING_COMPARE_OPERATE(<);
	DEF_STRING_COMPARE_OPERATE(==);
	DEF_STRING_COMPARE_OPERATE(!=);
#undef DEF_STRING_COMPARE_OPERATE

	private:
		friend class string_array;
		void call_constructor();
		void call_constructor(const string& v);

		union private_data
		{
			unsigned char size;
			struct  
			{
				unsigned char	size_for_local_data;
				char			local_data[0x1f];
			};
			struct
			{
				unsigned char	type_for_ref;
				ref_string		ref_data;
			};

		}	_private_data;

		static const int max_local_data_capacity = sizeof(private_data)-1-1;
		static const int private_data_type_for_ref = 0xff;
	};


	//�ر�򵥵��ַ�������
	class string_array
	{
	public:
		string_array();
		string_array(const string_array& v);
		~string_array();

		void			clear();
		string_array&	assign(const string_array& v);
		string_array&	operator =(const string_array& v);

		string_array&	push(const string& data);//ѹ��
		string			pop();//����

		const string&	at(int index) const;
		string&			edit(int index);

		inline const string&	operator[](int index)const{return at(index);}

		int				capacity() const;
		int				size()const;
		string*			resize(int count,bool keep_data=true);

		string			to_string( const string& seperate) const;
	private:

		struct header:public ref_memory::header
		{
			int			capacity;//��������string������
			int			count;//string������
			string		data[4];

			long		release();
		};

		header*			_data;
		string			_dummy;

		static const int extern_header_size = sizeof(header)-sizeof(string)*4;
	};
};