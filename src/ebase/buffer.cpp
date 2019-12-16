#include "buffer.hpp"
#include "atomic.hpp"
#include "precompile.h"
#include "allocator.hpp"
#include "macro/dblist.h"
#include <string.h>
namespace ebase
{
#define BUFFER_SHORT_HEADER_SIZE (buffer::header_size-sizeof(ref_memory::header))


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
        if(this->size()==0)
        {
            return this->assign(v);
        }else
        {
		    return this->append(v.data(),v.size());
        }
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
/**@brief 调整数据区大小
@param[in] data_size 调整数据区大小,保证对内存的独占引用
@param[in] keep_data 是否保持数据，如果false,而不保证原数据有效性
@return 返回可写数据区指针
@note 如果reset(0)且非独占引用,解除对内存的引用并返回回为0
*/

	void* buffer::resize(int size,bool keep_data)
	{
        int capacity = SIZE_ALIGN( size + buffer::header_size,64 )-sizeof( ref_memory::header );
        if(0==size)capacity = 0;

        int keep_size = 0;
        if(keep_data)keep_size = min(size,this->size())+BUFFER_SHORT_HEADER_SIZE;

        header* p = (header*)this->_data.resize( capacity ,keep_size );
        if(!p)return 0;

		p->_next = 0;
		p->_prev = 0;
		p->size = size;

		return p->data;
	}
/**@brief 调整缓冲区大小
@param[in] data_size 调整缓冲区大小,调整数据长度为0,保证对内存的独占引用
@return 返回可写数据区指针
*/
    void* buffer::alloc(int capacity)
    {
        void* p = this->resize(capacity,false );

        this->_data._data->size = 0;

        return p;
    }
/**@brief扩大缓冲区空间
@details 扩大缓冲区空间,空间至少扩大为this->size()+grown_capacity,返回扩大以后的空间,即返回当前数据区后面grown_capacity字节可写内存
@param[in] grown_capacity 扩大的空间大小,如果为0,不扩大空间
@return 返回this->data()+this->size()处的指针
@note 如果this->size()+grown_capacity>this->capacity() 会扩大空间,如果this->size()+grown_capacity<=this->capacity()则不会扩大空间
*/
    void* buffer::grown(int grown_capacity /*= 0 */)
    {
        assert(grown_capacity>=0);

        int old_size=this->size();
        char* p = (char*)this->resize(old_size+grown_capacity);

        this->_data._data->size = old_size;

        return p+old_size;
    }

    void buffer::cut_data(int cut_size)
    {
        if(this->size()==0)return;
        if(cut_size>=this->size())
        {
            this->resize(0);
            return;
        }
        
        int new_size = this->size()-cut_size;
		header* p = (header*)_data._data;

        if(p->is_alone())
        {
            memmove( p->data,p->data+cut_size,new_size);
		    p->size = new_size;
        }else
        {
            int capacity = SIZE_ALIGN( new_size + buffer::header_size,64 )-sizeof( ref_memory::header );

			p = (header*)ref_memory::header::alloc( capacity );
			
			memcpy(p->data,(char*)this->data()+cut_size,new_size );

			p->_next = 0;
			p->_prev = 0;
			p->size = new_size;
            this->_data.assign( p );
            p->release();
        }
    }

    /**@brief 解除对内存的引用
@details 如果内存引用次数为1，解除引用后，将会自动 释放掉内存
*/
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

    int buffer::find(const void* data,int len,int startpos) const
    {
        if(this->size()==0)return -1;
        if(0>=len)return -1;
        if(0>=startpos)startpos=0;

        const char* begin = (const char*)this->data();
        const char* end = begin+this->size()-len+1;

        const char* p = begin+startpos;

        while( p < end )
        {
            if(memcmp( p,data,len )==0)return int(p-begin);
            ++p;
        }

        return -1;
    }

    buffer::header* buffer::probe_unlist_header() const
	{
		header* p = (header*)_data._data;
		if(!p || p->size == 0)return 0;

        
		if( DBLIST_IS_IN_LIST(p))
		{
			int size = this->size();

            int capacity = SIZE_ALIGN( size + buffer::header_size,64 )-sizeof( ref_memory::header );

			p = (header*)ref_memory::header::alloc( capacity );
			
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
		_data_size = 0;
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