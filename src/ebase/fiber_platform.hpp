#pragma once
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <ucontext.h>
#endif
namespace ebase
{

    namespace platform
    {
#ifdef _WIN32

        typedef LPVOID      fiber_t;
#else
        typedef ucontext_t  fiber_t;
#endif

        typedef         void (*lpfn_fiber_entry)();

        bool            fiber_init(fiber_t* handle,lpfn_fiber_entry entry,void* params,int stack_size);    
        void            fiber_uninit(fiber_t* handle);
        bool            fiber_getcontext(fiber_t* out_current_handle);//获取当前context,会将线程的context保存在out_current_handle
        bool            fiber_setcontext(fiber_t* handle);
        bool            fiber_swapcontext(fiber_t* handle,fiber_t* out_current_handle);
    };
};