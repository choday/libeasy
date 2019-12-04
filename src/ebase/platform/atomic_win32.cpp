#include "../atomic.hpp"
#include <windows.h>
#include <intrin.h>


namespace ebase
{

	long atomic::increment(long volatile* p)
	{
		return _InterlockedIncrement(p);
	}


	long atomic::decrement(long volatile* p)
	{
		return _InterlockedDecrement(p);
	}

	long atomic::fetch_and_or(long volatile* p,long value)
	{
		return _InterlockedOr( p,value );
	}

	long atomic::fetch_and_and(long volatile* p,long value)
	{
		return _InterlockedAnd( p,value );
	}

	long atomic::fetch_and_xor(long volatile* p,long value)
	{
		return _InterlockedXor( p,value );
	}

	long atomic::fetch_and_add(long volatile* p,long value)
	{
		return _InterlockedExchangeAdd(p,value);
	}

	long atomic::fetch_and_sub(long volatile* p,long value)
	{
		return _InterlockedExchangeAdd(p,-value);
	}

	void* atomic::set_pointer(void** target,void* value)
	{
		return InterlockedExchangePointer(target,value);
	}

    void* atomic::compare_and_set_pointer(void** target,void* cmpvalue,void* value)
    {
        return ::InterlockedCompareExchangePointer( target,value,cmpvalue );
    }


}