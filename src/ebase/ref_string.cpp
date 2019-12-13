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
/**@brief 调整数据区大小
@param[in] size 调整数据区大小,保证对内存的独占引用
@param[in] keep_data 是否保持数据，如果false,而不保证原数据有效性
@return 返回可写数据区指针
@note 如果reset(0)且非独占引用,解除对内存的引用并返回回为0
*/
	char* ref_string::resize(int size,bool keep_data)
	{
        header* oldp = (header*)this->_data._data;
        header* p=0;
        if(size<0)size=0;

        if(0==this->_data._data && 0== size )return 0;//借用一下_size,返回为一个指向0的字符串

        int capacity = SIZE_ALIGN( size+1 + ref_string::header_size,32 )-sizeof( ref_memory::header );

        int keep_data_size = 0;
        if(keep_data&&size)keep_data_size = min(size,this->size())+( ref_string::header_size-sizeof( ref_memory::header ) );

        if(this->_offset>0 && keep_data_size )
        {
            if(oldp->is_alone())
            {
                memmove( oldp->data,oldp->data+this->_offset,keep_data_size );
            }else
            {
                oldp->add_ref();
				
                p = (header*)this->_data.resize( capacity,0 );//重新分配内存
				memcpy(p->data,oldp->data+this->_offset,keep_data_size );

                oldp->release();
            }
        }

        if(0==size)capacity = 0;
        if(!p)p = (header*)this->_data.resize( capacity,keep_data_size );
        if(!p)return 0;

        if(p)p->data[size] = 0;
	    this->_offset=0;
		this->_size = size;

        return p->data;
	}

	ebase::ref_string::header* ref_string::get_header() const
	{
		header* oldp = (header*)this->_data._data;
		if(oldp)oldp->add_ref();
		return oldp;
	}

};