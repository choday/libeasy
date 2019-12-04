#pragma once
#include "../../ebase/ref_class.hpp"
#include "../../ebase/executor.hpp"
#include "../../ebase/error.hpp"
#include "../../ebase/buffer.hpp"
#include "../../ebase/atomic.hpp"

#include "io_callback.hpp"
namespace eio
{

/*
实现io_request有2种方式
分为内部实现者与外部实现者

内部实现者，需要直接继承io_request,并且实现internal_do_request,或者直接实现do_request函数
外部实现者，需要设置_request_callback，相当于重写internal_do_request函数，两者相等


internal_do_request与do_request的区别，两者区别在于:
do_completed为start直接调用的函数，返回值将传递传递给start,
internal_do_request为在_request_executor中执行的函数

*/
	class io_request:public ebase::runnable
	{
	public:
		io_request();
		~io_request();
	public://使用者
		bool				start();

		ebase::error		error;//如果任务被取消，ERROR_OPERATION_ABORTED

		io_callback			completed_callback;
		ebase::executor*	completed_executor;

	public://实现者
		void				do_completed();
		virtual bool		do_request();

		io_callback			_request_callback;
		ebase::executor*	_request_executor;//如果不设置的话，当前线程中直接调用_request_callback
		bool				_call_completed_later;//internal_do_request/_request_callback,是否自动调用do_completed


	protected://内部实现者，直接从io_request继承,实现以下两个函数
		virtual void		internal_do_request();//会自动调用do_completed
	private:
		virtual void		run() override;
		ebase::atomic_flags	flags;//如果任务取消，请重置flags

	};
};