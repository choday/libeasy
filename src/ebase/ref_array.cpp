#include "ref_array.hpp"
#include "atomic.hpp"
namespace ebase
{

    void ref_array::init()
    {
        _header=0;
    }

    void ref_array::clear()
    {
        if(_header)_header->release(true);
        _header=0;
    }

    void ref_array::attach(struct header* h)
    {
        clear();
        if(h)
        {
            _header = h;
            h->add_ref();
        }
    }

    long ref_array::header::add_ref()
    {
        return atomic::increment(&ref_count);
    }

    long ref_array::header::release(bool auto_delete/*=true*/)
    {
        long result = atomic::decrement(&ref_count);
        if(auto_delete && result == 0 )
        {
            delete this;
        }
        return result;
    }

};