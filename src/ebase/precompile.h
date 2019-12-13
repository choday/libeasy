#pragma once



#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>


#if defined(_MSC_VER)
#include "precompile_msvc.h"
#else
#include <stdint.h>
#endif

#include "compile.h"

//feature
#ifdef _DEBUG
#define enable_allocator_log_count 1
#define counter_internal_allocate 1
#endif

#ifndef NOMINMAX
#define NOMINMAX //disable min/max macro for windows.h
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define SIZE_ALIGN(size,alignment) ((((size)/(alignment))*(alignment))+(((size)%(alignment))?(alignment):0))

#ifndef __weak
#define __weak __attribute__((weak))
#endif


#ifndef ELIB_API
#define ELIB_API
#endif
