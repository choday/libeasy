#pragma once
#include "runnable.hpp"
#include "thread.hpp"
namespace ebase
{
	class executor
	{
	public:
		executor();
		~executor();

		bool				post( runnable* ptr,bool wakeup=true );//一个runnable在同一时间只能post到一个executor

        virtual bool        abort(runnable* ptr);//取消单个runnable任务，如果runnable在其它的executor队列,而非本队列，会导致内存破坏
		virtual int			abort_all();//取消所有任务,//executor::abort_all被执行,每一个队列中的runnable将被调用
        bool                is_aborted();

    protected:
        bool                clear_abort_flag();//abort_all之后，重置环境，可以再次使用,如果当前为aborted状态，返回true

		virtual int			dispath();
        virtual int			dispath_batch();//适用于非阻塞模式,比如事件通知
		virtual void		need_dispath()=0;

	protected:
		static __thread executor*		_current;
		mutex_lock_t<runnable_queue>	_queue;
		atomic_flags					_flags;
        ref_ptr<runnable>               pop_runnable();
        int                             pop_runnable_batch(ref_ptr<runnable>* list,int maxsize);

        enum
        {
            status_flag_aborted = 1<<0,//已被取消
            status_flag_disable_dispath = 1<<2,//for executor_single_thread,只允许单线程执行
        };
	};

	//只允许一个线程执行
	class executor_single_thread:public runnable,public executor
	{
	public:
		executor_single_thread(ref_class_i* outref = nullptr);
		~executor_single_thread();

		void            set_parent(executor* p );

		virtual void    need_dispath() override;

	protected:
		virtual void    run() override;
	private:
		executor*		_parent;
	};
};