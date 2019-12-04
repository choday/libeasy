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
ʵ��io_request��2�ַ�ʽ
��Ϊ�ڲ�ʵ�������ⲿʵ����

�ڲ�ʵ���ߣ���Ҫֱ�Ӽ̳�io_request,����ʵ��internal_do_request,����ֱ��ʵ��do_request����
�ⲿʵ���ߣ���Ҫ����_request_callback���൱����дinternal_do_request�������������


internal_do_request��do_request������������������:
do_completedΪstartֱ�ӵ��õĺ���������ֵ�����ݴ��ݸ�start,
internal_do_requestΪ��_request_executor��ִ�еĺ���

*/
	class io_request:public ebase::runnable
	{
	public:
		io_request();
		~io_request();
	public://ʹ����
		bool				start();

		ebase::error		error;//�������ȡ����ERROR_OPERATION_ABORTED

		io_callback			completed_callback;
		ebase::executor*	completed_executor;

	public://ʵ����
		void				do_completed();
		virtual bool		do_request();

		io_callback			_request_callback;
		ebase::executor*	_request_executor;//��������õĻ�����ǰ�߳���ֱ�ӵ���_request_callback
		bool				_call_completed_later;//internal_do_request/_request_callback,�Ƿ��Զ�����do_completed


	protected://�ڲ�ʵ���ߣ�ֱ�Ӵ�io_request�̳�,ʵ��������������
		virtual void		internal_do_request();//���Զ�����do_completed
	private:
		virtual void		run() override;
		ebase::atomic_flags	flags;//�������ȡ����������flags

	};
};