#include<iostream>
#include<gtest/gtest.h>

#include <ebase/fiber_platform.hpp>


TEST(fiber,all)
{
    /*
    ebase::platform::fiber_t fiber_current;
    ebase::platform::fiber_t fiber_test;

    struct test
    {
        static void __stdcall fiber_entry(void* p )
        {

            //EXPECT_TRUE(p!=0);

            ebase::platform::fiber_setcontext((ebase::platform::fiber_t *) p );
            return;
        }
    };

    ebase::platform::fiber_getcontext(&fiber_current);
    ebase::platform::fiber_init( &fiber_test,(ebase::platform::lpfn_fiber_entry)&test::fiber_entry,&fiber_current,1 );

    ebase::platform::fiber_setcontext( &fiber_test );
    */

}