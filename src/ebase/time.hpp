#pragma once
#include <stdint.h>

namespace ebase
{

    class time
    {
    public:
        time();
        ~time();

        static uint64_t    get_tick_count(bool high_resolution=false);
    };
};