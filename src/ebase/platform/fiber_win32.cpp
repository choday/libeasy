#include "../fiber_platform.hpp"

namespace ebase
{
    namespace platform
    {

        bool fiber_init(fiber_t* handle,lpfn_fiber_entry entry,void* params,int stack_size)
        {
            *handle = CreateFiber( stack_size,(LPFIBER_START_ROUTINE)entry,params );
            return *handle != 0;
        }

        void fiber_uninit(fiber_t* handle)
        {
            DeleteFiber(*handle);
        }

        bool fiber_swapcontext(fiber_t* handle,fiber_t* out_current_handle)
        {
            if(!fiber_getcontext(out_current_handle))return false;
            fiber_setcontext(handle);
            return true;
        }
        
        bool fiber_getcontext(fiber_t* out_current_handle)
        {
            *out_current_handle = ConvertThreadToFiberEx(0,FIBER_FLAG_FLOAT_SWITCH);//保存当前线程的fiber,用SwitchToFiber恢复
            return *out_current_handle != 0;
        }     

        bool fiber_setcontext(fiber_t* handle)
        {
            SwitchToFiber(*handle);
            return true;
        }
    }
};