#include<iostream>
#include<gtest/gtest.h>
#include <ebuffer/ebuffer.hpp>

TEST(ebuffer,construct)
{

    class pack:public ebuffer::ebuffer_t<pack>
    {
    public:
        pack(){
        };

        ebuffer::value_node tt;
        ebuffer::value_node tt2;

        void assert_gtest()
        {
            ASSERT_TRUE(node_child==&tt );
            ASSERT_TRUE(tt.node_next==&tt2 );

        }
    };

    class pack2:public ebuffer::ebuffer_t<pack2>
    {
    public:
        pack2()
        {
            
        }

        ebuffer::value_node tt;
        pack                pp;
        pack                pp2;
        ebuffer::value_node tt2;

        void assert_gtest()
        {
            ASSERT_TRUE(node_next==0);
            ASSERT_TRUE(node_child==&tt );

            ASSERT_TRUE(tt.node_next==&pp );
            ASSERT_TRUE(pp.node_next==&pp2 );
            ASSERT_TRUE(pp2.node_next==&tt2 );
            ASSERT_TRUE(tt2.node_next==0 );

            pp.assert_gtest();
            pp2.assert_gtest();
        }
    };

    pack2 test,test2;

    test.assert_gtest();
    test2.assert_gtest();


}
