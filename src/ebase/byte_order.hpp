#pragma once
#include "precompile.h"

namespace ebase
{

namespace byte_order
{

	#define byte_order_swap16(A)        (((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8)))


	#define byte_order_swap32(A)        (	((((uint32_t)(A) & 0xff000000) >> 24) | \
										(((uint32_t)(A) & 0x00ff0000) >> 8) | \
										(((uint32_t)(A) & 0x0000ff00) << 8) | \
										(((uint32_t)(A) & 0x000000ff) << 24)) )

	#define byte_order_swap64(x)	((uint64_t)( \
									(uint64_t)(((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) | \
									(uint64_t)(((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
									(uint64_t)(((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
									(uint64_t)(((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) <<  8) | \
									(uint64_t)(((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >>  8) | \
									(uint64_t)(((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
									(uint64_t)(((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
									(uint64_t)(((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56) ))

	inline bool is_big_endian_host()
	{
		unsigned short _bigendian = 0x00FF;
		return 0 == *(unsigned short*)&_bigendian;
	}

	inline char				to_bigendian(char input){return input;}
	inline unsigned char	to_bigendian(unsigned char input){return input;}
	inline int16_t			to_bigendian(int16_t input){if(is_big_endian_host())return input;return byte_order_swap16(input);}
	inline uint16_t			to_bigendian(uint16_t input){return to_bigendian((uint16_t)input);}
	inline int32_t			to_bigendian(int32_t input){if(is_big_endian_host())return input;return byte_order_swap32(input);}
	inline uint32_t			to_bigendian(uint32_t input){return to_bigendian((int32_t)input);}
	inline int64_t			to_bigendian(int64_t input){if(is_big_endian_host())return input;return byte_order_swap64(input);}
	inline uint64_t			to_bigendian(uint64_t input){return to_bigendian((int64_t)input);}
};
};