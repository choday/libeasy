#pragma once

#include "thread_platform.hpp"

namespace ebase
{

	class mutex_lock
	{
	public:
		mutex_lock();
		~mutex_lock();

		void lock();
		bool trylock();
		void unlock();
	private:
		platform::mutex_lock_t	_handle;
	};

	class mutex_rwlock
	{
	public:
		mutex_rwlock();
		~mutex_rwlock();

		inline void lock(){this->lock_exclusive();}
		inline void unlock(){this->unlock_exclusive();}

		void lock_exclusive();//排它锁
		void unlock_exclusive();//排它锁

		void lock_shared();//共享锁
		void unlock_shared();//共享锁

	private:
		platform::mutex_rwlock_t		_handle;
	};

	class event_signal
	{
	public:
		event_signal();
		~event_signal();

		bool wait_event(unsigned long timeout_ms =-1 );
		void fire_event();
	private:
		platform::event_signal_t _handle;
	};

	class thread
	{
	public:
		thread();
		~thread();

		typedef int (*thread_proc)(void* param);

		static bool		begin_thread(thread_proc fun,void* param = 0);
		static void		sleep(int timeout_ms);
		static int		get_cpu_count();

		virtual void	run() = 0;
		bool			start(int thread_count=1,bool joinable=true);//如果thread_count==0，则thread_count=get_cpu_count()
        bool            is_started();

		int				get_active_count();
		bool			join();//等待所有线程结束,并关闭句柄，多个线程，无返回值,忽略outcode
	protected:
		platform::thread_t		_handle;
		bool					_joinable;
		long volatile			_thread_count;
		event_signal			_wait_join_event;
	private:
		static int	_thread_entry(void* param);
	};

	template<class _PARENT>
	class mutex_lock_t:public mutex_lock,public _PARENT
	{
	public:

	};

	class mutex_lock_scope
	{
	public:
		mutex_lock_scope(mutex_lock* lock);
		mutex_lock_scope(mutex_rwlock* lock);
		~mutex_lock_scope();

	private:
		mutex_lock*	_lock_ptr;
		mutex_rwlock*	_rwlock_ptr;
	};

	class shared_lock_scope
	{
	public:
		shared_lock_scope(mutex_rwlock* lock);
		~shared_lock_scope();

	private:
		mutex_rwlock*	_lock_ptr;
	};

	class exclusive_lock_scope
	{
	public:
		exclusive_lock_scope(mutex_rwlock* lock);
		~exclusive_lock_scope();

	private:
		mutex_rwlock*	_lock_ptr;
	};
}

