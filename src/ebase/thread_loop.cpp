#include "thread_loop.hpp"

namespace ebase
{

    thread_loop::thread_loop():_batch_mode(false)
	{

	}

    thread_loop::~thread_loop()
    {
        this->abort_all();
        this->join();
    }

    bool thread_loop::start(int thread_count,bool joinable)
	{
        executor::clear_abort_flag();
        if(thread::is_started())return true;

		return thread::start(thread_count,joinable);
	}

	void thread_loop::run()
	{
		while(!is_aborted())
		{
			_signal.wait_event( 1000 );
			if(_batch_mode)this->dispath_batch();
            else this->dispath();
            this->dispath_timer();
		}

        this->need_dispath();//为了让多线程能相继退出，这里需要持续设置信号，
	}

	void thread_loop::need_dispath()
	{
		_signal.fire_event();
	}

    void thread_loop::set_batch_mode(bool batch/*=true*/)
    {
        _batch_mode=true;
    }

    void thread_loop::need_dispath_timer()
    {
        this->need_dispath();
    }

};