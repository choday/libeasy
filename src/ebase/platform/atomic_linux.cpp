#include "../atomic.hpp"

namespace ebase
{
	//__sync_val_compare_and_swap
	//__sync_lock_test_and_set

	long atomic::increment(long volatile* p)
	{
		return __sync_add_and_fetch(p,1);
	}


	long atomic::decrement(long volatile* p)
	{
		return __sync_sub_and_fetch(p,1);
	}


 
	long atomic::fetch_and_or(long volatile* p,long value)
	{
		return __sync_fetch_and_or( p,value );
	}

	long atomic::fetch_and_and(long volatile* p,long value)
	{
		return __sync_fetch_and_and( p, value);
	}

	long atomic::fetch_and_xor(long volatile* p,long value)
	{
		return __sync_fetch_and_xor( p, value);
	}

	long atomic::fetch_and_add(long volatile* p,long value)
	{
		return __sync_fetch_and_add( p,value );
	}

	long atomic::fetch_and_sub(long volatile* p,long value)
	{
		return __sync_fetch_and_sub( p,value );
	}


	void* atomic::set_pointer(void** target,void* value)
	{
        return __sync_lock_test_and_set(target,value);
	}

    void* atomic::compare_and_set_pointer(void** target,void* cmpvalue,void* value)
    {
        return __sync_val_compare_and_swap ( target,cmpvalue,value );
    }
}