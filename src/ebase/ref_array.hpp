#pragma once

namespace ebase
{

    struct ref_array
    {
        struct header
        {

            long            add_ref();
            long            release(bool auto_delete=true);

            long volatile   ref_count;
            int             capacity;
            int             size;
            int             offset;
            //element_type    data[1];
        };

        struct header*  _header;

        void            init();
        void            clear();
        void            attach(struct header* h);
    };
};