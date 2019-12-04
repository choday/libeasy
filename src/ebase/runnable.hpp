#pragma once
#include "ref_class.hpp"
namespace ebase
{

	class runnable:public ref_class<>
	{
	public:
		runnable(ref_class_i* outref = nullptr);
		
        virtual void	on_abort();//executor::abort_all被执行,每一个队列中的runnable将被调用

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

		bool						push_back(runnable* ptr);//一个runnable只能在同一时间放入一个runnable_queue,否则返回false
		ebase::ref_ptr<runnable>	pop_front();
		bool						remove(runnable* ptr);//从队列中移除
		void						clear();//清空列表
		int							size() const;

		ref_ptr<runnable>			front();
	private:
			
		ebase::ref_ptr<runnable>	_first;
		runnable *					_last;
		
		int			_count;
	};
};