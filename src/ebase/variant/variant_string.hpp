#pragma once

namespace ebase
{
    struct variant_string
    {
        union
        {
            struct
            {
                unsigned char	        type;
                unsigned char	        string_type;
                char                    data[14];
            };
            struct
            {
                unsigned char	        type2;
                unsigned char	        string_type2;
                void*                   string_data;
            };
        };
    };
};