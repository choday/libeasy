#include "../thread_platform.hpp"
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#define gettid() syscall(__NR_gettid)
#define gettidv2() syscall(SYS_gettid)

namespace ebase
{
	namespace platform
	{

		void mutex_lock_init(mutex_lock_t* mutext_)
		{
			pthread_mutexattr_t mutex_attribute;
			pthread_mutexattr_init(&mutex_attribute);
			pthread_mutexattr_settype(&mutex_attribute, PTHREAD_MUTEX_RECURSIVE_NP );//嵌套锁，允许同一个线程对同一个锁成功获得多次，并通过多次unlock解锁。如果是不同线程请求，则在加锁线程解锁时重新竞争。
			pthread_mutex_init(mutext_, &mutex_attribute);
			pthread_mutexattr_destroy(&mutex_attribute);		
		}

		void mutex_lock_uninit(mutex_lock_t* mutext_)
		{
			pthread_mutex_destroy(mutext_);
		}

		bool mutex_lock_lock(mutex_lock_t* mutext_,bool wait /*= true*/)
		{
			if(wait)
			{
				pthread_mutex_lock(mutext_);
				return true;
			}else
			{
				return 0==pthread_mutex_trylock(mutext_);
			}
			
		}

		void mutex_lock_unlock(mutex_lock_t* mutext_)
		{
			pthread_mutex_unlock(mutext_);
		}

		void mutex_rwlock_init(mutex_rwlock_t* rwlock_)
		{
			pthread_rwlockattr_t attr;
			bool read_preferred=false;//读优先

			pthread_rwlockattr_init(&attr);
			pthread_rwlockattr_setkind_np (&attr, read_preferred?PTHREAD_RWLOCK_PREFER_READER_NP:PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
			pthread_rwlock_init(rwlock_, &attr);
		}

		void mutex_rwlock_uninit(mutex_rwlock_t* rwlock_)
		{
			pthread_rwlock_destroy(rwlock_);
		}

		bool mutex_rwlock_lock_read(mutex_rwlock_t* rwlock_,bool wait /*= true */)
		{
			int result = 0;

			result = wait?pthread_rwlock_rdlock(rwlock_):pthread_rwlock_tryrdlock(rwlock_);

			return 0==result;
		}

		bool mutex_rwlock_lock_write(mutex_rwlock_t* rwlock_ ,bool wait/*=true*/)
		{
			int result = 0;

			result = wait?pthread_rwlock_wrlock(rwlock_):pthread_rwlock_trywrlock(rwlock_);

			return 0==result;
		}

		void mutex_rwlock_unlock_read(mutex_rwlock_t* rwlock_)
		{
			pthread_rwlock_unlock(rwlock_);
		}

		void mutex_rwlock_unlock_write(mutex_rwlock_t* rwlock_)
		{
			pthread_rwlock_unlock(rwlock_);
		}

		void event_signal_init(event_signal_t* event_)
		{
            pthread_condattr_t condattr;
            pthread_condattr_init(&condattr);
            pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC);//default CLOCK_REALTIME
			pthread_cond_init(&event_->cond_,&condattr);
            pthread_condattr_destroy(&condattr);

			pthread_mutexattr_t mutex_attribute;
			pthread_mutexattr_init(&mutex_attribute);
			pthread_mutexattr_settype(&mutex_attribute, PTHREAD_MUTEX_RECURSIVE_NP );
			pthread_mutex_init(&event_->mutex_, &mutex_attribute);
			pthread_mutexattr_destroy(&mutex_attribute);

            event_->has_signal_ = false;
		}

		void event_signal_uninit(event_signal_t* event_)
		{
            pthread_cond_broadcast(&event_->cond_);
			pthread_cond_destroy(&event_->cond_);
			pthread_mutex_destroy(&event_->mutex_);
		}

		void event_signal_post(event_signal_t* event_)
		{
			pthread_mutex_lock(&event_->mutex_);
            event_->has_signal_ = true;
			pthread_mutex_unlock(&event_->mutex_);
            int result = pthread_cond_signal(&event_->cond_);
		}


struct timespec get_time_abs(int timeout_ms)
{

	struct timespec abstime;

    clock_gettime(CLOCK_MONOTONIC,&abstime );
    // ms(毫秒),us(微秒),ns(纳秒),ps(皮秒)

    abstime.tv_sec += timeout_ms / 1000;
    abstime.tv_nsec += (timeout_ms % 1000)*1000000;
    if (abstime.tv_nsec > 1000000000)
    {
        abstime.tv_sec += 1;
        abstime.tv_nsec -= 1000000000;
    }
	return abstime;
}

		bool event_signal_wait(event_signal_t* event_,int timeout_ms)
		{
			int result = 0;

			pthread_mutex_lock(&event_->mutex_);
            if(!event_->has_signal_)
            {
			    if(-1==timeout_ms)
			    {
				    result = pthread_cond_wait(&event_->cond_,&event_->mutex_);
			    }else
			    {
				    struct timespec timeoutval = get_time_abs(timeout_ms);
				    result =pthread_cond_timedwait(&event_->cond_,&event_->mutex_,&timeoutval);
			    }
                
            }
            if (event_->has_signal_)result = 0;
            event_->has_signal_ = false;
            //110 ETIMEDOUT
			pthread_mutex_unlock(&event_->mutex_);
            return 0 == result;
		}

		void semaphore_init(semaphore_t* sem_,int initvalue,int maxvalue)
		{
			sem_init(sem_,0,initvalue);
		}

		void semaphore_uninit(semaphore_t* sem_)
		{
			sem_destroy(sem_);
		}

		void semaphore_post(semaphore_t* sem_)
		{
			sem_post( sem_ );
		}

		bool semaphore_wait(semaphore_t* sem_,int timeout_ms)
		{
			int result = 0;
			if(-1==timeout_ms)
			{
				result = sem_wait(sem_);
			}else
			{

				struct timespec timeoutval = get_time_abs(timeout_ms);

				result =sem_timedwait(sem_,&timeoutval);
			}
			return 0==result;
		}

		void thread_sleep(int timeout_ms)
		{
			if(timeout_ms%1000==0)
			{
				sleep(timeout_ms/1000);
			}else
			{
				usleep(timeout_ms*1000);
			}
		}

		void thread_exit(int exit_code)
		{
			pthread_exit(*(void**)&exit_code);
		}

		thread_t thread_current()
		{
			return pthread_self();
		}

		thread_id thread_current_id()
		{
			return gettid();
		}

		thread_t thread_start(lpfn_thread_entry entry,void* params,bool joinable)
		{
			pthread_attr_t attr;
			thread_t		result = 0;

			pthread_attr_init(&attr);
			if(!joinable)pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

			pthread_create(&result, &attr,(void  *(*)(void  *))entry, params );
			pthread_attr_destroy(&attr);

			return result;
		}

		void thread_dettach(thread_t thread_)
		{
			pthread_detach(thread_);
		}

		bool thread_join(thread_t thread_,int* poutcode)
		{
			if(0==thread_)return false;

			void* _retvalue = 0;
			int n = pthread_join( thread_,&_retvalue);
			if(poutcode)*poutcode=*(int*)&_retvalue;

			return 0==n;
		}

	};
}