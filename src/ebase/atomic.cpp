#include "atomic.hpp"

namespace ebase
{
	
atomic_flags::atomic_flags():value(0)
{

}

long atomic_flags::set(long mask)
{
	return atomic::fetch_and_or( &value,mask );
}

long atomic_flags::clear(long mask)
{
	return atomic::fetch_and_and( &value,~mask );
}

bool atomic_flags::test(long mask)
{
	return (mask & atomic::fetch_and_or(&value,0))==mask;
}

bool atomic_flags::test_bit_and_set(int pos)
{
	long mask = 1<<pos;
	long v = atomic::fetch_and_or(&value,mask );
	return (mask & v )!=0;
}

bool atomic_flags::clear_bit(int pos)
{
	long mask = 1<<pos;

	return ( mask & atomic::fetch_and_and( &value,~mask ) ) !=0;
}

void atomic_flags::reset(long v)
{
	value=v;
}

}