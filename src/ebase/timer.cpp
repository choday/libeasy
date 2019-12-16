#include "timer.hpp"
#include "executor.hpp"


namespace ebase
{
    
    timer::timer()
    {
        abs_timeout_ms=-1;
        ref_tree::entry::set_holder(this);
    }

    timer::~timer()
    {

    }


    void timer::kill()
    { 
        if(ref_tree::entry::in_rbtree())
        {
            timer_host::ref_tree2timer_host(ref_tree::entry::_tree_host)->remove_timer(this);
        }        
    }

    int timer::compare_rbtree_entry(entry* right_value)
    {
        timer* e2=(timer*)right_value;
		if(this->abs_timeout_ms<e2->abs_timeout_ms)return -1;
        if(this->abs_timeout_ms>e2->abs_timeout_ms)return 1;
        return 0;
    }

    int timer::compare_rbtree_find_value(void* pfind_value)
    {
        uint64_t* p =(uint64_t*)pfind_value;
		if(this->abs_timeout_ms<*p)return -1;
        if(this->abs_timeout_ms>*p)return 1;
        return 0;
    }

    timer_host::timer_host():_enable_lock(true),_high_resolution(false)
    {

    }

    timer_host::~timer_host()
    {
        clear();
    }

    bool timer_host::set_timer(timer* tt,uint64_t abs_time_ms)
    {
        bool result=false;
        bool need_notify = false;

        assert(tt);
        if(0==tt)return result;
  
        if(cache_first_timer == tt)
        {
            if( tt->abs_timeout_ms >= abs_time_ms )
            {
                tt->abs_timeout_ms = abs_time_ms;
                result=true;
                need_notify=true;
            }else
            {
                tt->abs_timeout_ms = abs_time_ms;

                if(_enable_lock)_lock.lock();
                {
                    result=this->_rbtree.insert_unique( tt );
                    if(result)cache_first_timer = 0;
                }
                if(_enable_lock)_lock.unlock();
            }

        }else 
        {
            if( tt->abs_timeout_ms >= abs_time_ms )need_notify=true;

            if(_enable_lock)_lock.lock();
            {
                if(tt->in_rbtree())this->_rbtree.remove( tt );
                tt->abs_timeout_ms = abs_time_ms;
                result=this->_rbtree.insert_unique( tt ); 
            }
            if(_enable_lock)_lock.unlock();
            
        }  

        if(need_notify)this->need_dispath_timer();

        return result;
    }

    bool timer_host::remove_timer(timer* tt)
    {
        assert(tt);
        if(0==tt)return false;
        
        if(_enable_lock)_lock.lock();
            if(cache_first_timer == tt )
            {
                cache_first_timer = 0;
            }else if(tt->in_rbtree())
            {
                this->_rbtree.remove( tt );
            }
        if(_enable_lock)_lock.unlock();

        return true;
    }

    int timer_host::clear()
    {
        int count = 0;
        if(_enable_lock)_lock.lock();
            cache_first_timer=0;
            count=this->_rbtree.clear();
        if(_enable_lock)_lock.unlock();
        return count;
    }

    int timer_host::size()
    {
        return _rbtree.size();
    }

    int timer_host::dispath_timer()
    {
        ref_ptr<timer> out_list[32];

        int count = 0;
        int result = 0;

        
        do
        {
            count = this->pop_timeout_batch( out_list,32 );

            for(int i =0;i<count;++i)
            {
                out_list[i]->fire();
                result++;
            }

        }while(count==32);
        
        return result;
    }

    int timer_host::pop_timeout_batch(ref_ptr<timer>* out_list,int max_count)
    {
        int result = 0;

        uint64_t current_tick = time::get_tick_count(_high_resolution);

        if(_enable_lock)_lock.lock();
        {
            if(!cache_first_timer)cache_first_timer = pop_rbtree(false);

            while(cache_first_timer&&cache_first_timer->abs_timeout_ms<=current_tick)
            {
                out_list[result++]=cache_first_timer;

                cache_first_timer = pop_rbtree(false);
            } 

        }
        if(_enable_lock)_lock.unlock();

        return result;
    }

    ebase::ref_ptr<ebase::timer> timer_host::pop_rbtree(bool lock)
    {
        ebase::ref_ptr<ebase::timer> result;
        if(_enable_lock&&lock)_lock.lock();
        {
            ref_tree::entry* first = this->_rbtree.begin();
            if( first )
            {
                if(first->has_list_child())first=first->list_child_first();

                result = (timer*)first;
                this->_rbtree.remove(first);
            }
        }
        if(_enable_lock&&lock)_lock.unlock();
        return result;
    }

    uint64_t timer_host::get_difftime_ms()
    {
        uint64_t abs_timeout_ms = get_abstime_ms();
        if(-1==abs_timeout_ms)return -1;

        uint64_t current_tick = time::get_tick_count(_high_resolution);

        if(current_tick<abs_timeout_ms)return (abs_timeout_ms-current_tick);

        return 0;
    }

    uint64_t timer_host::get_abstime_ms()
    {
       
        if(_enable_lock)_lock.lock();
        if(!cache_first_timer)cache_first_timer = pop_rbtree(false);
        if(_enable_lock)_lock.unlock();

        if(!cache_first_timer) return -1;

        return cache_first_timer->abs_timeout_ms;
    }

    void timer_host::set_high_resolution(bool high_resolution)
    {
        this->_high_resolution=high_resolution;
    }

    void timer_host::set_disable_lock(bool disable/*=true*/)
    {
        this->_enable_lock=!disable;
    }

}