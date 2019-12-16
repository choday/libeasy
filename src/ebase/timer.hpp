#pragma once
#include "stdint.h"
#include "event_emitter.hpp"
#include "time.hpp"
#include "ref_tree.hpp"
namespace ebase
{
    class executor;

    class timer:public event_emitter,private ref_tree::entry
    {
    public:
        timer();
        ~timer();

        inline void         set_holder(ref_class_i* event_source_and_out_ref){this->event_emitter::set_event_source(event_source_and_out_ref);}
        inline ref_class_i* get_holder(){return event_emitter::_event_source;}
		template<class type>
		type*       		get_holder(){if(!_holder)return 0;return (type*)event_emitter::_event_source;}

        inline uint64_t     get_abstime_ms(){return this->abs_timeout_ms;}
        void                kill();

    private:
        friend class timer_host;
        uint64_t            abs_timeout_ms;
		virtual int		    compare_rbtree_entry( entry* right_value ) override;
		virtual int		    compare_rbtree_find_value(void* pfind_value ) override;
    };

	class timer_host
	{
	public:
        timer_host();
        ~timer_host();

        //����Ѵ������޸�ʱ��
        inline bool         set_timer_difftime(timer* t,uint64_t diff_time_ms){return this->set_timer(t,time::get_tick_count(_high_resolution)+diff_time_ms);}

        bool                set_timer(timer* t,uint64_t abs_time_ms);//����Ѵ��ڣ����޸ĳ�ʱʱ��,abs_time_ms��time::get_tick_count()����ֵ

        bool                remove_timer(timer* t);


    public:
        int                 clear();
        int                 size();

		int			        dispath_timer();
        uint64_t            get_difftime_ms();//��ȡ����һ��timer��get_difftime_ms����֮���ʱ���ֵ,��λ����
        uint64_t            get_abstime_ms();

        void                set_high_resolution(bool high_resolution=true);
        void                set_disable_lock(bool disable=true);

        int                 pop_timeout_batch(ref_ptr<timer>* out_list,int max_count);
		virtual void		need_dispath_timer()=0;

    private:

        mutex_rwlock         _lock;
        bool                _enable_lock;
        bool                _high_resolution;
        ref_ptr<timer>      cache_first_timer;

        ref_tree            _rbtree;
        ref_ptr<timer>      pop_rbtree(bool lock);

        friend class timer;
        static inline timer_host* ref_tree2timer_host(ref_tree* p){return (timer_host*)((char*)p - (char*)(&((timer_host*)0)->_rbtree) );}
	};
};