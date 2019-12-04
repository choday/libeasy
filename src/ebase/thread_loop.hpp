#pragma once
#include "executor.hpp"
#include "thread.hpp"
#include "timer.hpp"
namespace ebase
{

	class thread_loop:public executor,public thread,public timer_host
	{
	public:
		thread_loop();
        ~thread_loop();

		virtual bool	start(int thread_count=1,bool joinable=true);
		virtual void	run() override;

        void            set_batch_mode(bool batch=true);
    protected:
        virtual void	need_dispath() override;
        virtual void	need_dispath_timer() override;
	private:
		event_signal	_signal;
        bool            _batch_mode;
	};
};