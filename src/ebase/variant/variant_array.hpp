#pragma once

namespace ebase
{
    struct variant_array
    {
        struct header
        {
            long volatile   ref_count;
            int             capacity;
            int             size;
        };

        unsigned char	    type;
        header*	            header;
    };
};