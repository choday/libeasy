#include<iostream>
#include<gtest/gtest.h>

#include <ebase/ref_function.hpp>

TEST(ref_function,all)
{

    class test:public ebase::ref_class<>
    {
    public:
        test():value(8)
        {
        }

        static int static_function(int add_value)
        {
            return 1+add_value;
        }

        int get_value(int add_value)
        {
            return this->value+add_value;
        }
        int value;
    };

    typedef void (test::*class_fun)();
    typedef void (*static_fun)();

    EXPECT_EQ( sizeof(static_fun),sizeof(void*) );
 
    ebase::ref_ptr<test> pp = new test();

    EXPECT_EQ( sizeof(ebase::ref_function::function_truck),sizeof(class_fun) );

    ebase::ref_function::function_truck f;
    EXPECT_EQ( (void*)&f._function_ptr,(void*)&f );
    EXPECT_EQ( (void*)&f._offset,((char*)&f)+sizeof(void*) );

}