#pragma once
#include "precompile.h"
extern "C"
{
#include "c/rbtree.h"
}
#include "variant/variant_array.hpp"
#include "variant/variant_map.hpp"
#include "variant/variant_string.hpp"

namespace ebase
{
	class variant
	{
	public:
		variant();
		~variant();

		enum variant_type
		{
            type_undefined=0,//Œ¥∂®“Â
			type_null,//ø’
			type_bool,
			type_int,
			type_int64_t,
			type_float,
			type_double,
			type_string,
			type_array,
			type_map,
		};

		union variant_value
		{
            struct
            {
                unsigned char	type;
                union
                {
			        bool		vbool;
			        int			vint;
			        int64_t		vint64_t;
			        float		vfloat;
			        double		vdouble;
                };
            };

		    variant_array        varray;
			variant_map		     vmap;
		};
	};

};