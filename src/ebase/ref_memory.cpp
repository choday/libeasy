#include "ref_memory.hpp"
#include "allocator.hpp"
#include "atomic.hpp"
#include "precompile.h"

namespace ebase
{
/**@brief �����ڴ�
@details ����һ�δ����Զ����ü������ڴ�,�ڲ�����mallocϵͳ����

@param[in] size �����ڴ�Ŀռ��С��������ͷ��ref_memory::headerռ�ÿռ�
@return ����ref_memory::headerָ��, ��ʼ���ü���Ϊ1
@note ���ص�header���������ʹ��ʱ��release
*/
	ref_memory::header* ref_memory::header::alloc(int size)
	{
        size += sizeof(header);
		struct header* p = (struct header*)allocator::malloc( size );
		p->init(size);
		return p;
	}
/**@brief ���·����ڴ�
@details ����һ�δ����Զ����ü������ڴ�,�ڲ�����reallocϵͳ����

@param[in] size �����ڴ�Ŀռ��С��������ͷ��ref_memory::headerռ�ÿռ�
@return ����ref_memory::headerָ��,���ü�������ԭֵ
@attention  ���ﷵ��void*��Ϊ��ʹ�����Ժ���Ҫheader->release
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
/**@brief �������ü���
@return �������ü������Ӻ��ֵ
*/
	long ref_memory::header::add_ref()
	{
		long result = atomic::increment(&this->ref_count);
		assert(result>1);
		return result;
	}
/**@brief �������ü���
@param[in] auto_free ������Ϊ0���Ƿ��ͷ��ڴ�,Ĭ��Ϊtrue
@return �������ü������ٺ��ֵ
*/
	long ref_memory::header::release(bool auto_free)
	{
		long result = atomic::decrement(&this->ref_count);
		if(0==result&&auto_free)allocator::free(this);
		return result;
	}
/**@brief ��ʼ��
@details ��ʼ��ref_memory
@param[in] p ���õ��ڴ�
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
/**@brief �����header�ڴ������
*/
	void ref_memory::clear()
	{
		if(this->_data)this->_data->release();
		this->_data = 0;
	}
/**@brief ������header
@details ����Ծ�header�����ã�������header
@param[in] p ���õ��µ��ڴ�
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

/**@brief ������С�����ؿ�д�ڴ�
@details �����ǰ�������ڴ棬���� ΪΨһ�����ߣ�������realloc������\n�����ǰ�ڴ��С��capacity_size�󣬶������κβ���
@param[in] capacity_size �ռ��С��������header��ռ�ֽ�����
@param[in] keep_data_size ����keep_data_size�ֽ����ݲ���\n���keep_data_size==0,������alloc��ͬ
@return ����headerָ��
@note ���reset(0)�ҷǶ�ռ����,������ڴ�����ò�����0
@attention  ���ﷵ��void*��Ϊ��ʹ�����Ժ���Ҫheader->release
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