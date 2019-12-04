#include "executor.hpp"
#include "atomic.hpp"
namespace ebase
{
	__thread executor* executor::_current = nullptr;

	executor::executor()
	{

	}

	executor::~executor()
	{
	}

	bool executor::post(runnable* ptr,bool wakeup)
	{
		bool result=false;
        if(!ptr)return result;

        if( _flags.test(status_flag_aborted) )return false;

		_queue.lock();
		result = _queue.push_back(ptr);
		_queue.unlock();
		
		if(wakeup)this->need_dispath();

		return result;
	}


    bool executor::abort(runnable* ptr)
    {
        bool result=false;
        if(!ptr)return result;

        ref_ptr<runnable> hold = ptr;

		_queue.lock();
        result = _queue.remove(ptr);
		_queue.unlock();

        hold->on_abort();

        return result;
    }

    int executor::abort_all()
	{
		int result=0;

        if(status_flag_aborted&_flags.set(status_flag_aborted))return 0;

        this->need_dispath();

		_queue.lock();
		
        ref_ptr<runnable> ptr = _queue.pop_front();
        while(ptr)
        {
            ptr->on_abort();
            ptr = _queue.pop_front();
        }

		_queue.unlock();

		return result;
	}

    bool executor::is_aborted()
    {
        return _flags.test(status_flag_aborted);
    }

    bool executor::clear_abort_flag()
    {
        return (_flags.clear(status_flag_aborted)&status_flag_aborted) !=0;
    }

    ref_ptr<ebase::runnable> executor::pop_runnable()
    {
        ref_ptr<runnable> ptr;

		_queue.lock();
		ptr = _queue.pop_front();
		_queue.unlock();

        return ptr;
    }
    
    int executor::pop_runnable_batch(ref_ptr<runnable>* list,int maxsize)
    {
        int result = 0;
		_queue.lock();
		for(int i =0;i<maxsize&&_queue.size()>0;++i)
        {

            list[result] = _queue.pop_front();
        }
		_queue.unlock();

        return result;
    }

    int executor::dispath()
	{
		ref_ptr<runnable> ptr;
		int count = 0;

        if(is_aborted())return 0;

		ptr = pop_runnable();
        while(ptr)
        {
            executor::_current=this;

            if( is_aborted() )
            {
                ptr->on_abort();
            }else
            {
			    ptr->run();
			    
            }
			count++;
            ptr = pop_runnable();
        }
		return count;
	}

    int executor::dispath_batch()
    {
        ref_ptr<runnable> ptr[32];
		int count = 0;
        int result=0;

        do 
        {
            result = pop_runnable_batch( ptr,32 );

            for( int i =0;i<result;++i)
            {
                if( is_aborted() )
                {
                     ptr[i]->on_abort();
                }else
                {
                    ptr[i]->run();
                }
            }
            count+=result;

        } while ( result==32 );
        

        count += dispath();

        return count;
    }

    executor_single_thread::executor_single_thread(ref_class_i* outref /*= nullptr*/):runnable(outref),_parent(0)
	{

	}

	executor_single_thread::~executor_single_thread()
	{

	}

	void executor_single_thread::set_parent(executor* p)
	{
		_parent = p;
	}

	void executor_single_thread::need_dispath()
	{
		if(_parent)_parent->post(this,true);
	}

	void executor_single_thread::run()
	{
		if(status_flag_disable_dispath & _flags.set(status_flag_disable_dispath))return;	
		this->dispath();
		_flags.clear(status_flag_disable_dispath);
	}
};