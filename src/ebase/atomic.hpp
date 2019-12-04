#pragma once

namespace ebase
{
    //InterlockedCompare64Exchange128�Ƚ�����˼
	class atomic
	{
	public:
		static long increment(long volatile* p);
		static long decrement(long volatile* p);


		//returns the original value stored in the variable pointed to by Destination
		static long fetch_and_or( long volatile* p,long value);
		static long fetch_and_and( long volatile* p,long value);
		static long fetch_and_xor( long volatile* p,long value);
		static long fetch_and_add( long volatile* p,long value);
		static long fetch_and_sub( long volatile* p,long value);

        //return old *target,���º���ָ������ڴ����(sizeof(void*))
        static void* set_pointer(void** target,void* value);
        static void* compare_and_set_pointer(void** target,void* cmpvalue,void* value);
        
	};

	class atomic_flags
	{
	public:
		atomic_flags();

		long	set(long mask);//return old value;
		long	clear(long mask);//return old value;
		bool	test(long mask);//test bits by mask,if all matched return true

		bool	test_bit_and_set(int pos);//����ԭ����ֵ
		bool	clear_bit(int pos);//����ԭ����ֵ

		void	reset(long v=0);//��0

		long volatile	value;
	};
};