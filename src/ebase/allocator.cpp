#include "allocator.hpp"
#include "atomic.hpp"
#include <malloc.h>

namespace ebase
{



	allocator::lpfn_malloc allocator::malloc_handle=&allocator::default_malloc;
	allocator::lpfn_realloc allocator::realloc_handle=&allocator::default_realloc;
	allocator::lpfn_free allocator::free_handle=&allocator::default_free;

#if (defined(enable_allocator_log_count) && enable_allocator_log_count)
	long allocator::allocated_count_ = 0;
#endif

	void* allocator::default_malloc(size_t nSize)
	{
		return ::malloc(nSize);
	}

	void* allocator::default_realloc(void* p,size_t nSize)
	{
		return ::realloc(p,nSize);
	}

	void allocator::default_free(void* p)
	{
		return ::free(p);
	}


	void allocator::set_malloc_handle(lpfn_malloc fn_malloc,lpfn_realloc fn_realloc,lpfn_free fn_free)
	{
		if( fn_malloc&&fn_free&&fn_realloc )
		{
			malloc_handle = fn_malloc;
			realloc_handle = fn_realloc;
			free_handle = fn_free;
		}else
		{
			malloc_handle=allocator::default_malloc;
			realloc_handle=allocator::default_realloc;
			free_handle=allocator::default_free;
		}
	}

	long allocator::get_allocated_count()
	{
#if (defined(enable_allocator_log_count) && enable_allocator_log_count)
		return allocated_count_;
#endif
		return 0;
	}

	void* allocator::malloc(size_t nSize)
	{
		void* p =malloc_handle(nSize);
#if (defined(enable_allocator_log_count) && enable_allocator_log_count)
		if( p )atomic::increment( &allocated_count_ );
#endif
		return p;
	}

	void* allocator::realloc(void* p,size_t nSize)
	{
		if(0==p)return allocator::malloc(nSize);
		if(0==nSize)
		{
			allocator::free(p);
			return 0;
		}


		return realloc_handle(p,nSize);
	}

	void allocator::free(void* p)
	{
		free_handle(p);
#if (defined(enable_allocator_log_count) && enable_allocator_log_count)
		if( p )atomic::decrement( &allocated_count_ );
#endif
	}

	

	class_allocator::class_allocator()
	{
	}

	void* class_allocator::operator new( size_t cb )
	{
		void* p =  allocator::malloc( cb );
		return p;
	}

	void * class_allocator::operator new[]( size_t cb )
	{
		void* p= allocator::malloc( cb );
		return p;
	}

	void class_allocator::operator delete( void* p )
	{
		allocator::free(p);
		
	}

	void class_allocator::operator delete[]( void* p )
	{
		allocator::free(p);
	}

}