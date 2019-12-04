#include "../thread_platform.hpp"
/*
BOOLEAN TryAcquireSRWLockExclusive(
  PSRWLOCK SRWLock
);
BOOLEAN TryAcquireSRWLockShared(
  PSRWLOCK SRWLock
);*/
namespace ebase
{
	namespace platform
	{

		void mutex_lock_init(mutex_lock_t* mutext_)
		{
			::InitializeCriticalSection(mutext_);
		}

		void mutex_lock_uninit(mutex_lock_t* mutext_)
		{
			::DeleteCriticalSection(mutext_);
		}

		bool mutex_lock_lock(mutex_lock_t* mutext_,bool wait /*= true*/)
		{
			if(wait)
			{

				EnterCriticalSection(mutext_);
			}else
			{
				return 0!=TryEnterCriticalSection(mutext_);
			}
			return true;
		}

		void mutex_lock_unlock(mutex_lock_t* mutext_)
		{
			LeaveCriticalSection(mutext_);
		}

		void mutex_rwlock_init(mutex_rwlock_t* rwlock_)
		{
			InitializeSRWLock(rwlock_);
		}

		void mutex_rwlock_uninit(mutex_rwlock_t* rwlock_)
		{

		}

		bool mutex_rwlock_lock_read(mutex_rwlock_t* rwlock_,bool wait /*= true */)
		{
#if defined(_MSC_VER) && _MSC_VER<1600
			AcquireSRWLockShared(rwlock_);
			return true;
#else
			if(wait)
			{
				AcquireSRWLockShared(rwlock_);
				return true;
			}
			return 0!=TryAcquireSRWLockShared(rwlock_);//Kernel32.dll
#endif
		}

		bool mutex_rwlock_lock_write(mutex_rwlock_t* rwlock_ ,bool wait/*=true*/)
		{
#if defined(_MSC_VER) && _MSC_VER<1600
				AcquireSRWLockExclusive(rwlock_);
				return true;
#else
			if(wait)
			{
				AcquireSRWLockExclusive(rwlock_);
				return true;
			}

			return 0!=TryAcquireSRWLockExclusive(rwlock_);
#endif
		}

		void mutex_rwlock_unlock_read(mutex_rwlock_t* rwlock_)
		{
			ReleaseSRWLockShared(rwlock_);
		}

		void mutex_rwlock_unlock_write(mutex_rwlock_t* rwlock_)
		{
			ReleaseSRWLockExclusive(rwlock_);
		}

		void event_signal_init(event_signal_t* event_)
		{
			*event_ = CreateEvent( 0,0,0,0 );
		}

		void event_signal_uninit(event_signal_t* event_)
		{
			if(event_)CloseHandle(*event_);
		}

		void event_signal_post(event_signal_t* event_)
		{
			SetEvent( *event_ );
		}

		bool event_signal_wait(event_signal_t* event_,int timeout_ms)
		{
			return WAIT_OBJECT_0==WaitForSingleObject( *event_,timeout_ms );
		}

		void semaphore_init(semaphore_t* sem_,int initvalue,int maxvalue)
		{
			*sem_ = CreateSemaphore( 0,initvalue,65535,0 );
		}

		void semaphore_uninit(semaphore_t* sem_)
		{
			CloseHandle(*sem_);
		}

		void semaphore_post(semaphore_t* sem_)
		{
			ReleaseSemaphore( sem_,1,0 );
		}

		bool semaphore_wait(semaphore_t* sem_,int timeout_ms)
		{
			return WAIT_OBJECT_0==WaitForSingleObject( *sem_,timeout_ms );
		}

		void thread_sleep(int timeout_ms)
		{
			Sleep(timeout_ms);
		}

		void thread_exit(int exit_code)
		{
			ExitThread(exit_code);
		}

		thread_t thread_current()
		{
			return GetCurrentThread();
		}

		thread_id thread_current_id()
		{
			return GetCurrentThreadId();
		}

		thread_t thread_start(lpfn_thread_entry entry,void* params,bool joinable)
		{
			HANDLE h = CreateThread( 0,0,(LPTHREAD_START_ROUTINE)entry,params,0,0 );
			if(h && !joinable)
			{
				CloseHandle(h);
				return h;
			}

			return h;
		}

		void thread_dettach(thread_t thread_)
		{
			if(thread_)CloseHandle(thread_);
		}

		bool thread_join(thread_t thread_,int* poutcode)
		{
			if( thread_ && WAIT_OBJECT_0 == ::WaitForSingleObject( thread_,-1 ) )
			{
				DWORD code = 0;
				GetExitCodeThread( thread_,&code );
				CloseHandle(thread_);
				if(poutcode)*poutcode = (int)code;
				return true;
			}
			return false;
		}

	};
}