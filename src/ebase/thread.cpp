#include "thread.hpp"
#include "atomic.hpp"
#include "platform.hpp"
namespace ebase
{

	mutex_lock::mutex_lock()
	{
		platform::mutex_lock_init( &_handle );
	}

	mutex_lock::~mutex_lock()
	{
		platform::mutex_lock_uninit( &_handle );
	}

	void mutex_lock::lock()
	{
		platform::mutex_lock_lock( &_handle,true );
	}

	bool mutex_lock::trylock()
	{
		return platform::mutex_lock_lock( &_handle,false );
	}

	void mutex_lock::unlock()
	{
		platform::mutex_lock_unlock( &_handle );
	}


	mutex_rwlock::mutex_rwlock()
	{
		platform::mutex_rwlock_init(&_handle);
	}

	mutex_rwlock::~mutex_rwlock()
	{
		platform::mutex_rwlock_uninit(&_handle);
	}

	void mutex_rwlock::lock_shared()
	{
		platform::mutex_rwlock_lock_read(&_handle);
	}

	void mutex_rwlock::lock_exclusive()
	{
		platform::mutex_rwlock_lock_write(&_handle);
	}

	void mutex_rwlock::unlock_shared()
	{
		platform::mutex_rwlock_unlock_read(&_handle);
	}

	void mutex_rwlock::unlock_exclusive()
	{
		platform::mutex_rwlock_unlock_write(&_handle);
	}



	event_signal::event_signal()
	{
		platform::event_signal_init(&_handle);
	}



	event_signal::~event_signal()
	{
		platform::event_signal_uninit(&_handle);
	}


	bool event_signal::wait_event(unsigned long timeout_ms)
	{
		return platform::event_signal_wait(&_handle,timeout_ms);
	}

	void event_signal::fire_event()
	{
		platform::event_signal_post(&_handle);
	}


	thread::thread():_handle(0),_joinable(false),_thread_count(0)
	{
	
	}

	thread::~thread()
	{
		if(_joinable && _handle )platform::thread_dettach(_handle);
		_handle = 0;
	}


	bool thread::begin_thread(thread_proc fun,void* param /*= 0*/)
	{
		return platform::thread_start(fun,param,false)!=0;
	}

	void thread::sleep(int timeout_ms)
	{
		platform::thread_sleep( timeout_ms );
	}

	bool thread::start(int thread_count/*=1*/,bool joinable)
	{
		bool result = false;
		_joinable = joinable;

		if(0==thread_count)thread_count=this->get_cpu_count();

		if(thread_count==1)
		{
			_handle = platform::thread_start(&thread::_thread_entry,this,joinable);
			result = (_handle!=0);

		}else for(int i =0;i<thread_count;++i)
		{
            _handle=(platform::thread_t)-1;
			if( thread::begin_thread( &thread::_thread_entry,this )  )result=true;
            
		}

        if(!result)_handle=0;
		return result;
	}

    bool thread::is_started()
    {
        return _joinable || _handle || _thread_count;
    }

    int thread::get_active_count()
	{
		return _thread_count;
	}

	int thread::get_cpu_count()
	{
		return platform::get_cpu_count();
	}

	bool thread::join()
	{
		if(!_joinable)return true;
        
		bool result = false;
        if(((platform::thread_t)-1)==_handle)
        {
            result = _wait_join_event.wait_event(-1);
        }else if(_handle)
		{
			result = platform::thread_join(_handle);
		}else
		{
			
		}
		_joinable=false;
        _handle=0;
		return result;
	}

	int thread::_thread_entry(void* param)
	{
		thread* pthis = (thread*)param;

		atomic::increment(&pthis->_thread_count);
		pthis->run();
        long current_count=atomic::decrement(&pthis->_thread_count);
		if(0==current_count && pthis->_joinable && ((platform::thread_t)-1)==pthis->_handle )
		{
			pthis->_wait_join_event.fire_event();
		}
		return 0;
	}

	mutex_lock_scope::mutex_lock_scope(mutex_lock* lock)
	{
		_rwlock_ptr = 0;
		_lock_ptr=lock;
		_lock_ptr->lock();
	}

	mutex_lock_scope::mutex_lock_scope(mutex_rwlock* lock)
	{
		_lock_ptr=0;

		_rwlock_ptr = lock;
		_rwlock_ptr->lock();
	}

	mutex_lock_scope::~mutex_lock_scope()
	{
		if(_lock_ptr)_lock_ptr->unlock();
		if(_rwlock_ptr)_rwlock_ptr->unlock();

		_lock_ptr=0;
		_rwlock_ptr = 0;
	}

	shared_lock_scope::shared_lock_scope(mutex_rwlock* lock)
	{
		_lock_ptr=lock;
		_lock_ptr->lock_shared();
	}

	shared_lock_scope::~shared_lock_scope()
	{
		_lock_ptr->unlock_shared();
		_lock_ptr=0;
	}

	exclusive_lock_scope::exclusive_lock_scope(mutex_rwlock* lock)
	{
		_lock_ptr=lock;
		_lock_ptr->lock_exclusive();
	}

	exclusive_lock_scope::~exclusive_lock_scope()
	{
		_lock_ptr->unlock_exclusive();
		_lock_ptr=0;
	}

};