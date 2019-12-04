#include<iostream>
#include<gtest/gtest.h>
#include <ebase/atomic.hpp>
#include <ebase/timer.hpp>
#include <ebase/thread.hpp>
#include <set>

TEST(timer,all)
{
    class tt_host:public ebase::timer_host 
    {
    public:
        virtual void		need_dispath_timer() override
        {
        }

        uint64_t value;
        
    } host;

    const int timer_count = 100;
    ebase::ref_ptr<ebase::timer>     tt[timer_count];

    for(int i = 0;i<timer_count;++i)
    {
        tt[i] = new ebase::timer();

        host.set_timer(tt[i],i);
    }

    ASSERT_EQ(host.size(),timer_count);

    ebase::ref_ptr<ebase::timer> out_time[timer_count];

    int count = host.pop_timeout_batch(out_time,timer_count);
    EXPECT_EQ(count,timer_count);
    for(int i =0;i<count;++i)
    {
        EXPECT_EQ(i,out_time[i]->get_abstime_ms());
    }

  ASSERT_EQ(host.size(),0);

    for(int i = 0;i<10;++i)
    {
        host.set_timer(tt[i],i);
    }
ASSERT_EQ(host.size(),10);

    host.set_timer(tt[2],2);
    host.set_timer(tt[3],2);
    host.set_timer(tt[4],2);
    host.set_timer(tt[5],2);

    count = host.pop_timeout_batch(out_time,timer_count);
    EXPECT_EQ(count,10);

    EXPECT_EQ(2,out_time[2]->get_abstime_ms());
    EXPECT_EQ(2,out_time[3]->get_abstime_ms());
    EXPECT_EQ(2,out_time[4]->get_abstime_ms());
    EXPECT_EQ(2,out_time[5]->get_abstime_ms());

    for(int i = 0;i<timer_count;++i)
    {
        //十个定时 器时间值
        host.set_timer(tt[i],(rand()*10)/RAND_MAX );
    }

    ASSERT_EQ(host.size(),timer_count);

    count = host.pop_timeout_batch(out_time,timer_count);
    ASSERT_EQ(host.size(),0);
    EXPECT_EQ(count,timer_count);
    uint64_t max_value = -1;

    std::set<uint64_t> testset;

    for(int i =0;i<count;++i)
    {
        testset.insert(out_time[i]->get_abstime_ms());
    }

    EXPECT_LE( (int)testset.size(),10 );


    host.clear();

    for(int i = 0;i<timer_count;++i)
    {
        //十个定时 器时间值
        host.set_timer(tt[i],rand()%10 );
    }


    ASSERT_EQ( host.clear(),timer_count );

}