#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#else

#include <unistd.h>
#include <pthread.h>
#include "semaphore.h"
#include "sys/time.h"
//#include <sys/syscall.h>
#endif

namespace ebase
{

	namespace platform
	{
#ifdef _WIN32
		typedef CRITICAL_SECTION	mutex_lock_t;
		typedef SRWLOCK				mutex_rwlock_t;
		typedef HANDLE				thread_t;
		typedef DWORD				thread_id;
		typedef HANDLE				event_signal_t;
		typedef HANDLE				semaphore_t;
#else

#if defined(_COMPILER_MINGW)
enum
{
	PTHREAD_RWLOCK_PREFER_READER_NP,
	PTHREAD_RWLOCK_PREFER_WRITER_NP,
	PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
	PTHREAD_RWLOCK_DEFAULT_NP = PTHREAD_RWLOCK_PREFER_READER_NP
};
extern int pthread_rwlockattr_setkind_np (pthread_rwlockattr_t *__attr,int __pref);
#define gettid() 0 //syscall(SYS_gettid)
#endif

		struct _event_signal_t_
		{
			pthread_cond_t cond_;
			pthread_mutex_t mutex_;
            bool            has_signal_;//万年巨坑,垃圾平台
		};

		typedef pthread_mutex_t			mutex_lock_t;
		typedef pthread_rwlock_t		mutex_rwlock_t;
		typedef pthread_t				thread_t;
		typedef pid_t					thread_id;
		typedef sem_t					semaphore_t;
		typedef struct _event_signal_t_ event_signal_t;

#endif
		void			mutex_lock_init(mutex_lock_t* mutext_);
		void			mutex_lock_uninit(mutex_lock_t* mutext_);
		bool			mutex_lock_lock(mutex_lock_t* mutext_,bool wait = true);
		void			mutex_lock_unlock(mutex_lock_t* mutext_);

		void			mutex_rwlock_init(mutex_rwlock_t* rwlock_);
		void			mutex_rwlock_uninit(mutex_rwlock_t* rwlock_);
		bool			mutex_rwlock_lock_read(mutex_rwlock_t* rwlock_,bool wait = true );
		bool			mutex_rwlock_lock_write(mutex_rwlock_t* rwlock_ ,bool wait=true);
		void			mutex_rwlock_unlock_read(mutex_rwlock_t* rwlock_);
		void			mutex_rwlock_unlock_write(mutex_rwlock_t* rwlock_);

		void			event_signal_init(event_signal_t* event_);
		void			event_signal_uninit(event_signal_t* event_);
		void			event_signal_post(event_signal_t* event_);//发送信号
		bool			event_signal_wait(event_signal_t* event_,int timeout_ms);//timeout_ms=-1 forever,return true if has signal and not timeout

		void			semaphore_init(semaphore_t* sem_,int initvalue);
		void			semaphore_uninit(semaphore_t* sem_);
		void			semaphore_post(semaphore_t* sem_);
		bool			semaphore_wait(semaphore_t* sem_,int timeout_ms);//timeout_ms=-1 forever,return true if has signal and not timeout

		void			thread_sleep( int timeout_ms );

		void			thread_exit(int exit_code);//call in thread
		thread_t		thread_current();
		thread_id		thread_current_id();


		typedef int	(* lpfn_thread_entry)(void* params);

		thread_t		thread_start(lpfn_thread_entry entry,void* params,bool joinable=true);//失败返回0,成功返回非0
		void			thread_dettach(thread_t thread_);
		bool			thread_join(thread_t thread_,int* poutcode=0);

	};

};