#include "ref_string.hpp"
#include "precompile.h"
#include <string.h>

namespace ebase
{

    const int ref_string::header_size=sizeof(ref_string::header)-sizeof(void*);

    void ref_string::init(struct header* p)
	{
		this->_data.init(p);
		this->_offset = 0;
		this->_size = 0;
	}

	void ref_string::clear()
	{
		this->_data.clear();
		this->_offset = 0;
		this->_size = 0;
	}

	const char* ref_string::data() const
	{
		header* p =(header*)this->_data._data;
		if(!p)return 0;

		return p->data+this->_offset;
	}

	const char* ref_string::c_str() const
	{
        const char* p = this->data();
        if(p && p[this->size()] == 0 )return p;
		return ((ref_string*)this)->resize(this->size(),true);
	}

	int ref_string::size() const
	{
		return this->_size;
	}

	int ref_string::capacity() const
	{
		header* p =(header*)this->_data._data;
		if(!p)return 0;

		return p->total_size-header_size-this->_offset;
	}

	bool ref_string::assign(const ref_string& v,int offset,int size)
	{
		assert(offset>=0);
		assert(size>=0);

		if( v.size() > offset && offset>=0 && size>=0 )
		{
			if( 0 == size ) size = v.size()-offset;
			else size = min( size,v.size()-offset );

			this->_data.assign( v._data );
			this->_offset = v._offset+offset;
			this->_size = size;
		}else
		{
			this->clear();
		}
		return true;
	}

	bool ref_string::assign(const char* data,int size /*= 0*/)
	{
		if( data && 0==size)size=(int)strlen(data);
		void* p = this->resize(size,false);
		if(data)memcpy( p,data,(size_t)size );

		return true;
	}

	bool ref_string::append(const char* data,int size /*= 0*/)
	{
		if( data && 0==size)size=(int)strlen(data);
		if( data && size)
		{
			int origin_size = this->size();
			char* p = this->resize(origin_size+size,true);
			memcpy( p+origin_size,data,size );
		}
		return true;
	}

	char* ref_string::resize(int size,bool keep_data)
	{
        if(size<0)size=0;

		header* oldp = (header*)this->_data._data;
		header* p=0;

        int new_memory_size = size+1+ref_string::header_size;
        new_memory_size = ((new_memory_size>>5)<<5)+32;
        
        int keep_data_size = 0;
        if(keep_data)keep_data_size = min(size,this->_size);

		if( 0== oldp )
		{
			if(size>0)p = (header*)this->_data.alloc( new_memory_size );

		}else
		{
            if(this->_offset>0)
            {
			    long oldp_ref_count = oldp->add_ref();
                
                if( oldp_ref_count==2 )
                {
                    if(keep_data_size)memmove( oldp->data,oldp->data+this->_offset,keep_data_size );
                }else if( oldp_ref_count>2 && keep_data_size )
                {
				    p = (header*)this->_data.alloc( new_memory_size );
				    memcpy(p->data,oldp->data+this->_offset,keep_data_size );
                }else
                {
                    this->_data.clear();
                }

                this->_offset = 0;
                oldp->release();
            }


            p = (header*)this->_data.resize( new_memory_size,keep_data_size+header_size );

		}

        if(p)p->data[size] = 0;
	    this->_offset=0;
		this->_size = size;

		return p?p->data:((char*)&_size);//借用一下_size,返回为一个指向0的字符串
	}

	ebase::ref_string::header* ref_string::get_header() const
	{
		header* oldp = (header*)this->_data._data;
		if(oldp)oldp->add_ref();
		return oldp;
	}

};