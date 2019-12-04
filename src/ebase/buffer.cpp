#include "buffer.hpp"
#include "atomic.hpp"
#include "precompile.h"
#include "allocator.hpp"
#include "macro/dblist.h"
namespace ebase
{

    const int buffer::header_size = sizeof(buffer::header)-sizeof(void*);

    buffer::buffer()
	{
		_data.init();
	}

	buffer::buffer(const buffer& v)
	{
		_data.init();
		this->assign(v);
	}

	buffer::buffer(header* p)
	{
		_data.init();
		_data.assign(p);
	}

	buffer::~buffer()
	{
		_data.clear();
	}

	buffer& buffer::operator=(const buffer& v)
	{
		return this->assign(v);
	}

	buffer& buffer::assign(const buffer& v)
	{
		_data.assign( v._data );
		return *this;
	}

	buffer& buffer::assign(const void* data,int size)
	{
		if(0==data)return *this;
		void* p = this->resize(size,false);
		if(p)
		{
			memcpy( p,data,size );
		}
		return *this;
	}

	buffer& buffer::append(const buffer& v)
	{
		return this->append(v.data(),v.size());
	}

	buffer& buffer::append(const void* data,int size)
	{
		int origin_size = this->size();
		char* p = (char*)this->resize(origin_size+size,true);
		if(p)
		{
			memcpy( p+origin_size,data,size );
		}
		return *this;
	}

	void* buffer::resize(int size,bool keep_data)
	{
		header* p = (header*)_data._data;
			
        int keep_data_size = 0;

		if(size && keep_data && this->size() )keep_data_size=min(size,this->size());

		if( p && DBLIST_IS_IN_LIST(p) )
		{
			p = (header*)ref_memory::header::alloc( size+buffer::header_size );
			p->_next = 0;
			p->_prev = 0;
			p->size = size;

			if(keep_data_size)memcpy(p->data,this->data(),keep_data_size );

			this->_data.assign(p);
			p->release();
		}else
		{
			int capacity = max(size,32);
			capacity = max(this->capacity(),capacity);

			p = (header*)this->_data.resize( capacity+buffer::header_size,keep_data_size+buffer::header_size );

			p->_next = 0;
			p->_prev = 0;
			p->size = size;
		}
		return p->data;
	}

	void* buffer::alloc(int capacity )
	{
        header* p = (header*)this->_data.alloc( capacity+buffer::header_size );

		p->_next = 0;
		p->_prev = 0;
		p->size = 0;

		return p->data;
	}

	void buffer::clear()
	{
		_data.clear();
	}

	const void* buffer::data() const
	{
		header* p=(header*)_data._data;
		if(0==p)return 0;
		return p->data;
	}

	int buffer::size() const
	{
		header* p=(header*)_data._data;
		if(0==p)return 0;
		return p->size;
	}

	int buffer::capacity() const
	{
		header* p=(header*)_data._data;
		if(0==p)return 0;

		return p->total_size-header_size;
	}

	bool buffer::is_empty() const
	{
		header* p=(header*)_data._data;
		if(0==p)return true;
		if(0==p->size)return true;
		return false;
	}

	buffer::header* buffer::probe_unlist_header() const
	{
		header* p = (header*)_data._data;
		if(!p || p->size == 0)return 0;

		if( DBLIST_IS_IN_LIST(p))
		{
			int size = this->size();

			p = (header*)ref_memory::header::alloc( size+buffer::header_size );
			
			if(size)memcpy(p->data,this->data(),size );

			p->_next = 0;
			p->_prev = 0;
			p->size = size;
		}else
		{
			p->add_ref();
		}

		return p;
	}

	buffer buffer::next()
	{
		header* p = (header*)_data._data;
		return p->_next;
	}

	buffer_list::buffer_list()
	{
		_first=0;
		_last = 0;
		_count=0;
	}

	buffer_list::buffer_list(const buffer_list& v)
	{
		_first=0;
		_last = 0;
		_count=0;
		_data_size = 0;

		assign(v);
	}

	buffer_list::~buffer_list()
	{
		this->clear();
	}

	buffer_list& buffer_list::assign(const buffer_list& v)
	{
		for(buffer i = v.front();!i.is_empty();i=i.next())
		{
			this->push_back( i );
		}
        this->_data_size = v._data_size;
		return *this;
	}

	buffer_list& buffer_list::operator=(const buffer_list& v)
	{
		return assign(v);
	}

	void buffer_list::push_back(const buffer& input)
	{
		buffer::header* p = input.probe_unlist_header();
		if(!p)return;

		DBLIST_PUSH_BACK( this,p );
		_count++;
		_data_size += input.size();
	}

	void buffer_list::push_front(buffer& input)
	{
		buffer::header* p = input.probe_unlist_header();
		if(!p)return;

		DBLIST_PUSH_FRONT( this,p );
		_count++;
		_data_size += input.size();
	}

	buffer buffer_list::pop_front()
	{
		buffer entry = this->front();

		if(!DBLIST_IS_EMPTY(this))
		{
			DBLIST_POP_FRONT(this);

			entry._data._data->release();
			_count--;
			_data_size -= entry.size();
		}

		return entry;
	}

	buffer buffer_list::pop_back()
	{
		buffer entry = this->back();

		if(!DBLIST_IS_EMPTY(this))
		{
			DBLIST_POP_BACK(this);
			entry._data._data->release();
			_count--;
			_data_size -= entry.size();
		}

		return entry;
	}

	void buffer_list::clear()
	{
		while(this->_first)
		{
			buffer::header* entry = this->_first;
			DBLIST_POP_FRONT(this);
			entry->release();
		}

		_data_size = 0;
		_count = 0;
	}

	buffer buffer_list::front() const
	{
		return buffer( this->_first );
	}

	buffer buffer_list::back() const
	{
		return buffer( this->_last );
	}

	int buffer_list::count() const
	{
		return _count;
	}

	int buffer_list::data_size() const
	{
		return _data_size;
	}

}