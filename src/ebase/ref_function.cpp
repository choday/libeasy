#include "ref_function.hpp"
namespace ebase
{

    ref_function::ref_function(void** address_of_function,void* object_ptr,ref_class_i* holder)
	{
        this->_function_ptr._function_ptr = 0;
        this->_function_ptr._offset = 0;
		if(address_of_function)this->set(address_of_function,object_ptr,holder);
	}

    bool ref_function::isset()
    {
        return this->_function_ptr._function_ptr !=0;
    }

    void ref_function::set(void** address_of_function,void* object_ptr,ref_class_i* holder)
    {
        if(address_of_function)this->_function_ptr = *(function_truck*)address_of_function;
        this->_object_ptr = object_ptr;
        this->_holder = holder;
    }

    void ref_function::clear()
    {
        this->_function_ptr._function_ptr = 0;
        this->_function_ptr._offset = 0;
        this->_object_ptr=0;
        this->_holder.reset();
    }

    void ref_function::invoke()
	{
		if(_object_ptr)
		{
			typedef void (ref_function::*class_lpfn)();
			( (ref_function*)_object_ptr->*( *(class_lpfn*)&_function_ptr) )();
		}else
		{
			typedef void (*lpfn)();
			( *(lpfn*)&_function_ptr )();
		}
	}
};