#pragma once
#include "precompile.h"

namespace ebase
{
	class allocator
	{
	public:
		typedef void*	(*lpfn_malloc)( size_t nSize);
		typedef void*	(*lpfn_realloc)(void* p,size_t nSize);
		typedef void	(*lpfn_free)( void* p );

		static void		set_malloc_handle(lpfn_malloc fn_malloc,lpfn_realloc fn_realloc,lpfn_free fn_free );
		static long		get_allocated_count();

		static void*	malloc( size_t nSize );
		static void*	realloc(void* p,size_t nSize);
		static void		free( void* p );
	private:
		static lpfn_malloc		malloc_handle;
		static lpfn_realloc		realloc_handle;
		static lpfn_free		free_handle;

		static void*	default_malloc( size_t nSize );
		static void*	default_realloc(void* p,size_t nSize);
		static void		default_free( void* p );
#if (defined(enable_allocator_log_count) && enable_allocator_log_count)
		static long				allocated_count_;
#endif
	};

	class class_allocator
	{
	public:
		class_allocator();

		//new 允许返回0,如果返回0,则不调用构造函数
		void* operator new(size_t cb);
		void * operator new[]( size_t cb );
		void operator delete(void* p);
		void operator delete[](void* p);
	protected:
	};
};
