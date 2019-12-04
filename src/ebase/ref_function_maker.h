//function base 自动代码生成,去掉最后四行的注释,生成预处理文件即可生成代码
//请不要使用此文件干 生成代码以外的事

#include "macro/repeat.h"

#define __for_each_number_1_8(v, ...) \
    v(1, __VA_ARGS__) \
    v(2, __VA_ARGS__) \
    v(3, __VA_ARGS__) \
    v(4, __VA_ARGS__) \
    v(5, __VA_ARGS__) \
    v(6, __VA_ARGS__) \
    v(7, __VA_ARGS__) \
    v(8, __VA_ARGS__) \

#define __tparam_class_N(n, ...) class argu_type_##n,
#define __tparam_class_N_end(n, ...) class  argu_type_##n
#define repeat_class_N(n) __repeat(n,__tparam_class_N,__tparam_class_N_end)

#define __tparam_argu_type_N(n, ...) argu_type_##n argu_##n,
#define __tparam_argu_type_N_end(n, ...) argu_type_##n argu_##n
#define repeat_argu_type_N(n) __repeat(n,__tparam_argu_type_N,__tparam_argu_type_N_end)

#define __tparam_argu_N(n, ...) argu_##n,
#define __tparam_argu_N_end(n, ...) argu_##n
#define repeat_argu_N(n) __repeat(n,__tparam_argu_N,__tparam_argu_N_end)

#define repeat_define_call(n, ...)\
template< repeat_class_N(n) > void invoke( repeat_argu_type_N(n) )\
{\
    ref_class_i* pp = _object_ptr.get();\
	if(pp)\
	{\
		typedef void (ref_class_i::*class_lpfn)(repeat_argu_type_N(n));\
		( pp->*( *(class_lpfn*)&_function_ptr) )(repeat_argu_N(n));\
	}else\
	{\
		typedef void (*lpfn)(repeat_argu_type_N(n));\
		( ( *(lpfn*)&_function_ptr) )(repeat_argu_N(n));\
	}\
}
#define repeat_define_call_result(n, ...)\
template< class return_type,repeat_class_N(n) > return_type call_result( repeat_argu_type_N(n) )\
{\
    ref_class_i* pp = _object_ptr.get();\
	if(pp)\
	{\
		typedef return_type (ref_class_i::*class_lpfn)(repeat_argu_type_N(n));\
		return ( pp->*( *(class_lpfn*)&_function_ptr) )(repeat_argu_N(n));\
	}else\
	{\
		typedef return_type (*lpfn)(repeat_argu_type_N(n));\
		return ( ( *(lpfn*)&_function_ptr) )(repeat_argu_N(n));\
	}\
}


//实现
__for_each_number_1_8(repeat_define_call);
__for_each_number_1_8(repeat_define_call_result);
