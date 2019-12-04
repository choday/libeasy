#include "../fiber_platform.hpp"
#include "../allocator.hpp"
#include "../precompile.h"

//__thread      fiber_t*       thread_fiber_current = 0;
#define DEFAULT_FIBER_STACK_SIZE    4*1024
namespace ebase
{
    namespace platform
    {
#if !defined(__APPLE__) && !defined(__ANDROID__)
        bool fiber_init(fiber_t* handle,lpfn_fiber_entry entry,void* params,int stack_size)
        {
            if(0==stack_size)stack_size=DEFAULT_FIBER_STACK_SIZE;
            if( -1==getcontext(handle) )return false;
            handle->uc_stack.ss_sp = allocator::malloc(stack_size);
            handle->uc_stack.ss_size = stack_size;
            handle->uc_link = 0;//执行完以后，将回到uc_link
            if(!handle->uc_stack.ss_sp)return false;
            makecontext( handle,entry,1,params );
            return true;
        }

        void fiber_uninit(fiber_t* handle)
        {
            allocator::free(handle->uc_stack.ss_sp);
            handle->uc_stack.ss_sp=0;
            handle->uc_stack.ss_size = 0; 
        }

        bool fiber_swapcontext(fiber_t* handle,fiber_t* out_current_handle)
        {
            if( -1!=swapcontext( out_current_handle,handle  ) )return false;
            return true;
        }
        
        bool fiber_getcontext(fiber_t* out_current_handle)
        {
            if(-1==getcontext(out_current_handle) )return false;

            return true;
        }

        bool fiber_setcontext(fiber_t* handle)
        {

            if( -1!= setcontext(handle) )return false;


            return true;
        }
#endif
    }
};