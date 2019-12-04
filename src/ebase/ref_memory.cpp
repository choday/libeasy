#include "ref_memory.hpp"
#include "allocator.hpp"
#include "atomic.hpp"
#include "precompile.h"

namespace ebase
{
	ref_memory::header* ref_memory::header::alloc(int size)
	{
		struct header* p = (struct header*)allocator::malloc( size );
		p->init(size);
		return p;
	}

	ref_memory::header* ref_memory::header::realloc(int size)
	{
		assert(size);
		header* p = (header*)allocator::realloc( this,size );
        if(p)p->total_size = size;
        return p;
	}

	void ref_memory::header::init(int size)
	{
		this->ref_count=1;
        this->total_size = size;
	}

	long ref_memory::header::add_ref()
	{
		long result = atomic::increment(&this->ref_count);
		assert(result>1);
		return result;
	}

	long ref_memory::header::release(bool auto_free)
	{
		long result = atomic::decrement(&this->ref_count);
		if(0==result&&auto_free)allocator::free(this);
		return result;
	}

	void ref_memory::init(struct header* p)
	{
		
		if(p)
		{
			p->add_ref();
			this->_data = p;
		}else
		{
			this->_data=0;
		}
	}

	void ref_memory::clear()
	{
		if(this->_data)this->_data->release();
		this->_data = 0;
	}

	void ref_memory::assign(struct header* p)
	{
		if(p==this->_data)return ;

		clear();

		if(p)p->add_ref();
		this->_data=p;
	}

	void ref_memory::assign(const struct ref_memory& p)
	{
		this->assign( p._data );
	}

	bool ref_memory::is_empty() const
	{
		return _data==0;
	}

	void* ref_memory::alloc(int capacity_size)
	{
		header* p = header::alloc(capacity_size);
		this->assign(p);
		p->release();

		return p;
	}

	void* ref_memory::resize(int new_size,int keep_data_size/*=0*/)
	{
        keep_data_size = min(keep_data_size,sizeof(header));

		if(keep_data_size>new_size)keep_data_size=new_size;

		header* p=this->_data;
		if(!p)return this->alloc(new_size);

        if(p->total_size>=new_size)return p;

		long result = p->add_ref();
        p->release();

		if( result == 2 )
		{
            this->_data = (header*)allocator::realloc( p,new_size );
            p = this->_data;
            p->total_size = new_size;
		}else
		{
			p = header::alloc( new_size );
			    if(keep_data_size)memcpy(p,this->_data,keep_data_size );
			    this->assign(p);
			p->release();
		}
		return p;
	}
}