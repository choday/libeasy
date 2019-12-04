#include<iostream>
#include<gtest/gtest.h>
#include <ebase/atomic.hpp>
#include <ebase/time.hpp>
#include <ebase/thread.hpp>

#ifdef _WIN32

namespace ebase
{
    namespace platform
    {
        uint64_t   set_global_tick_base(uint32_t tick_count);
    }
}


TEST(platform,set_global_tick_base)
{

    ASSERT_EQ(ebase::platform::set_global_tick_base(1111),1111);
    ASSERT_EQ(ebase::platform::set_global_tick_base(0x00ffffff),0x00ffffff);
    ASSERT_EQ(ebase::platform::set_global_tick_base(0xeeffffff),0xeeffffff);
    ASSERT_EQ(ebase::platform::set_global_tick_base(0xffffffff),0xffffffff);
    ASSERT_EQ(ebase::platform::set_global_tick_base(0x0000ffff),0x10000ffff);

    uint64_t tt = ebase::time::get_tick_count(true);
    ebase::thread::sleep(1);
    uint64_t diff = ebase::time::get_tick_count(true)-tt;
    ASSERT_GE(tt,0);
}

#endif


TEST(time,get_tick_count)
{

}
