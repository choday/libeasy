#include<iostream>
#include<gtest/gtest.h>
#include <ebase/atomic.hpp>
#include <ebase/thread.hpp>

TEST(thread,mutex_rwlock2)
{

    class lock_thread :public ebase::thread
    {
    public:
        lock_thread(ebase::mutex_rwlock& lock) :_lock(lock)
        {
            value = 0;
        }

        int value;

        void run()
        {

            _lock.lock();
            value = 1;
            _lock.unlock();

            _event.fire_event();
        }

        ebase::event_signal	_event;
        ebase::mutex_rwlock& _lock;
        
    };


    ebase::mutex_rwlock lock;
    lock_thread		_lock_thread(lock);


    _lock_thread.start(1);
    _lock_thread._event.wait_event();

    lock.lock_shared();
    EXPECT_EQ(_lock_thread.value, 1);
    lock.unlock_shared();


    EXPECT_TRUE(_lock_thread.join());


}
TEST(thread,event_signal)
{
	ebase::event_signal	_event;

	EXPECT_FALSE( _event.wait_event(0) );

	_event.fire_event();

	EXPECT_TRUE( _event.wait_event(0) );
	EXPECT_FALSE( _event.wait_event(0) );
	
}

TEST(thread,mutex_lock)
{
	class lock_thread:public ebase::thread
	{
	public:
		lock_thread(ebase::mutex_lock& lock):_lock(lock)
		{
			value=0;
		}

		int value;

		void run()
		{
			_event.fire_event();

			_lock.lock();
			EXPECT_EQ(value,2);
			value=1;
			_lock.unlock();

			_event.fire_event();
		}

		ebase::event_signal	_event;
		ebase::mutex_lock& _lock;
	};

	ebase::mutex_lock lock;
	lock_thread		_lock_thread(lock);

	lock.lock();
	_lock_thread.start(1);
	_lock_thread._event.wait_event();
	_lock_thread.value = 2;
	lock.unlock();

    _lock_thread._event.wait_event();

    EXPECT_TRUE(lock.trylock());
	EXPECT_EQ(_lock_thread.value,1);
	lock.unlock();
	

	EXPECT_TRUE( _lock_thread.join() );

	ASSERT_EQ(_lock_thread.value,1);

}

TEST(thread,mutex_rwlock)
{
	class lock_thread:public ebase::thread
	{
	public:
		lock_thread(ebase::mutex_rwlock& lock):_lock(lock)
		{
			value=0;
			value2=0;
		}

		int value;
		int value2;

		void run()
		{
			_event.fire_event();

			_lock.lock_shared();
			EXPECT_EQ(value,1);
			value2=value;
			_lock.unlock_shared();

			_event.fire_event();
		}

		ebase::event_signal	_event;
		ebase::mutex_rwlock& _lock;
	};

	ebase::mutex_rwlock lock;

	lock_thread			_lock_thread(lock);

	lock.lock_exclusive();
	_lock_thread.start(1);
	_lock_thread._event.wait_event();
	_lock_thread.value=1;
	lock.unlock_exclusive();


	lock.lock_shared();

	_lock_thread._event.wait_event();
	EXPECT_EQ(_lock_thread.value2,1);

	lock.unlock_shared();

	EXPECT_TRUE( _lock_thread.join() );
}


TEST(thread,start)
{

	class test_count_thread:public ebase::thread
	{
	public:
		test_count_thread():value(0){}

		long	value;
		virtual void run() override
		{
			ebase::thread::sleep(100);//等待其它线程启动到来
			ebase::atomic::increment(&value);
            return;
		}
	};


	test_count_thread	_count_thread;

	_count_thread.start(1);
	

	EXPECT_TRUE( _count_thread.join() );
	EXPECT_EQ( ebase::atomic::increment(&_count_thread.value),2 );
	
	_count_thread.value = 0;
	_count_thread.start(5);
	EXPECT_TRUE( _count_thread.join() );
	EXPECT_EQ( ebase::atomic::increment(&_count_thread.value),6 );
	
	
};