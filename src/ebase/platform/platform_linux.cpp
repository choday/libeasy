#include "../platform.hpp"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
namespace ebase
{
	namespace platform
	{
        //    // ms(∫¡√Î),us(Œ¢√Î),ns(ƒ…√Î),ps(∆§√Î)
        uint64_t    get_tick_count(bool high_resolution)
        {
            struct timespec ts={0};
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
        }

		int get_cpu_count()
		{
			return sysconf(_SC_NPROCESSORS_ONLN);
		}

        int get_last_error()
        {
            return errno;
        }
	}
	

};