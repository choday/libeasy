#include "ref_memory.hpp"
#include "allocator.hpp"
#include "atomic.hpp"
#include "precompile.h"

namespace ebase
{
/**@brief 分配内存
@details 分配一段带有自动引用计数的内存,内部调用malloc系统函数

@param[in] size 分配内存的空间大小，不包含头部ref_memory::header占用空间
@return 返回ref_memory::header指针, 初始引用计数为1
@note 返回的header，如果不再使用时请release
*/
	ref_memory::header* ref_memory::header::alloc(int size)
	{
        size += sizeof(header);
		struct header* p = (struct header*)allocator::malloc( size );
		p->init(size);
		return p;
	}
/**@brief 重新分配内存
@details 分配一段带有自动引用计数的内存,内部调用realloc系统函数

@param[in] size 分配内存的空间大小，不包含头部ref_memory::header占用空间
@return 返回ref_memory::header指针,引用计数保持原值
@attention  这里返回void*意为着使用完以后不需要header->release
*/
	void* ref_memory::header::realloc(int size)
	{
		assert(size);
        size += sizeof(header);
		header* p = (header*)allocator::realloc( this,size );
        if(p)p->total_size = size;
        return p;
	}

    bool ref_memory::header::is_alone()
    {
        return 1 == atomic::fetch_and_or( &this->ref_count,0 );
    }

    void ref_memory::header::init(int size)
	{
		this->ref_count=1;
        this->total_size = size;
	}
/**@brief 增加引用计数
@return 返回引用计数增加后的值
*/
	long ref_memory::header::add_ref()
	{
		long result = atomic::increment(&this->ref_count);
		assert(result>1);
		return result;
	}
/**@brief 减少引用计数
@param[in] auto_free 当计数为0后，是否释放内存,默认为true
@return 返回引用计数减少后的值
*/
	long ref_memory::header::release(bool auto_free)
	{
		long result = atomic::decrement(&this->ref_count);
		if(0==result&&auto_free)allocator::free(this);
		return result;
	}
/**@brief 初始化
@details 初始化ref_memory
@param[in] p 引用的内存
*/
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
/**@brief 解除对header内存的引用
*/
	void ref_memory::clear()
	{
		if(this->_data)this->_data->release();
		this->_data = 0;
	}
/**@brief 引用新header
@details 解除对旧header的引用，引用新header
@param[in] p 引用的新的内存
*/
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

/**@brief 调整大小并返回可写内存
@details 如果当前已引用内存，并且 为唯一引用者，将进行realloc操作。\n如果当前内存大小比capacity_size大，而不做任何操作
@param[in] capacity_size 空间大小，不包含header所占字节数量
@param[in] keep_data_size 保持keep_data_size字节数据不变\n如果keep_data_size==0,则功能与alloc相同
@return 返回header指针
@note 如果reset(0)且非独占引用,解除对内存的引用并返回0
@attention  这里返回void*意为着使用完以后不需要header->release
*/
	void* ref_memory::resize(int capacity_size,int keep_data_size/*=0*/)
	{
        if(keep_data_size>capacity_size)keep_data_size=capacity_size;

		if(0==this->_data)
        {
            if(0==capacity_size)return 0;
            this->_data = header::alloc(capacity_size);

        }else if( this->_data->is_alone() )
		{
            if( this->_data->total_size>=capacity_size+sizeof(header) )return this->_data;

            this->_data = (header*)this->_data->realloc(capacity_size);
		}else
		{
            if(0==capacity_size)
            {
                this->clear();
                return 0;
            }
			header* p = header::alloc( capacity_size );
			if(keep_data_size)memcpy(p+1,this->_data+1,keep_data_size );
			this->_data->release();
            this->_data=p;
		}
		return this->_data;
	}
}