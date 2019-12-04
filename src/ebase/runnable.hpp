#pragma once
#include "ref_class.hpp"
namespace ebase
{

	class runnable:public ref_class<>
	{
	public:
		runnable(ref_class_i* outref = nullptr);
		
        virtual void	on_abort();//executor::abort_all��ִ��,ÿһ�������е�runnable��������

		virtual void	run() = 0;

		bool			is_schedule();
	private:
		friend class runnable_queue;
		runnable*					_prev;
		ebase::ref_ptr<runnable>	_next;
	};

	class runnable_queue
	{
	public:
		runnable_queue();
		~runnable_queue();

		bool						push_back(runnable* ptr);//һ��runnableֻ����ͬһʱ�����һ��runnable_queue,���򷵻�false
		ebase::ref_ptr<runnable>	pop_front();
		bool						remove(runnable* ptr);//�Ӷ������Ƴ�
		void						clear();//����б�
		int							size() const;

		ref_ptr<runnable>			front();
	private:
			
		ebase::ref_ptr<runnable>	_first;
		runnable *					_last;
		
		int			_count;
	};
};