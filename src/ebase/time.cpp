#include "time.hpp"
#include <time.h>
#include "platform.hpp"
namespace ebase
{

    time::time()
    {

    }

    time::~time()
    {

    }

    uint64_t time::get_tick_count(bool high_resolution)
    {
        return platform::get_tick_count(high_resolution);
    }

}