#pragma once
#include "ref_class.hpp"
namespace ebase
{
	class argus_packet
	{
	public:
		virtual void emit(void* function_ptr,void* object_ptr=0) = 0;


		static argus_packet* make()
		{
			class make_0:public argus_packet
			{
			public:
				virtual void emit(void* function_ptr,void* object_ptr=0) override
				{
					if(object_ptr)
					{
						typedef void (argus_packet::*class_lpfn)();
						( (argus_packet*)object_ptr->*( *(class_lpfn*)&function_ptr) )();
					}else
					{
						typedef void (*lpfn)();
						( *(lpfn*)&function_ptr )();
					}
				}
			};
			return new make_0();
		}

		template<typename argu_type_0>
		static ref_ptr<argus_packet> make(const argu_type_0 argu_0 )
		{

			class make_1:public argus_packet
			{
			public:
				make_1(const argu_type_0& _argu_0):argu_0(_argu_0),argu_count(1){}
				argu_type_0 argu_0;
				int			argu_count;
			};
			return new make_0(argu_0);
		}
	};

	class argus_packet_ref
	{
	public:


	};
};