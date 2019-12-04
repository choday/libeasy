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

		bool				post( runnable* ptr,bool wakeup=true );//һ��runnable��ͬһʱ��ֻ��post��һ��executor

        virtual bool        abort(runnable* ptr);//ȡ������runnable�������runnable��������executor����,���Ǳ����У��ᵼ���ڴ��ƻ�
		virtual int			abort_all();//ȡ����������,//executor::abort_all��ִ��,ÿһ�������е�runnable��������
        bool                is_aborted();

    protected:
        bool                clear_abort_flag();//abort_all֮�����û����������ٴ�ʹ��,�����ǰΪaborted״̬������true

		virtual int			dispath();
        virtual int			dispath_batch();//�����ڷ�����ģʽ,�����¼�֪ͨ
		virtual void		need_dispath()=0;

	protected:
		static __thread executor*		_current;
		mutex_lock_t<runnable_queue>	_queue;
		atomic_flags					_flags;
        ref_ptr<runnable>               pop_runnable();
        int                             pop_runnable_batch(ref_ptr<runnable>* list,int maxsize);

        enum
        {
            status_flag_aborted = 1<<0,//�ѱ�ȡ��
            status_flag_disable_dispath = 1<<2,//for executor_single_thread,ֻ�����߳�ִ��
        };
	};

	//ֻ����һ���߳�ִ��
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