#pragma once
#if defined(_MSC_VER)

#define __thread __declspec( thread )
#define __weak __declspec(selectany) 

#if defined(_ELIB_EXPORTS_)
#define ELIB_API  __declspec(dllexport)
#endif

#if _MSC_VER < 1600 //msvc2010

#define nullptr 0

#else//msvc2010


#endif//msvc2010




#endif//#if defined(_MSC_VER)