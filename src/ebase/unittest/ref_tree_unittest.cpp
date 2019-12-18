#include<iostream>
#include<gtest/gtest.h>

#include <ebase/ref_tree.hpp>


TEST(ref_tree,ref_tree)
{

	class entry_node:public ebase::ref_tree::entry,public ebase::ref_class<>
	{
	public:
		entry_node(int v):value(v){
            ebase::ref_tree::entry::set_holder(this);
        }

		virtual void		on_destroy(){
            return ;
        }

		virtual int		compare_rbtree_entry( ebase::ref_tree::entry* right_value )
		{
			entry_node* p = (entry_node*)right_value;

			if(this->value<p->value)return -1;
			if(this->value>p->value)return 1;

			return 0;
		}

		virtual int		compare_rbtree_find_value( void* find_value )
		{
			int aaa=*(int*)&find_value;

			if(this->value<aaa)return -1;
			if(this->value>aaa)return 1;

			return 0;
		}

		int value;
	};

	ebase::ref_tree tree;

	entry_node		a1(1),a2(2),a3(3),a4(4),a5(5);
	entry_node		b2(2);

	EXPECT_TRUE( tree.insert_equal(&a1) );
	entry_node* pp1 = (entry_node*)tree.find((void*)1);
	ASSERT_TRUE(pp1!=0);
	EXPECT_EQ(pp1->value,1);

	EXPECT_FALSE( tree.insert_equal(&a1) );
	ASSERT_TRUE( a1.ref_count() == 1 );
	EXPECT_TRUE( tree.remove(&a1) );
	ASSERT_TRUE( a1.ref_count() == 0 );

	EXPECT_TRUE( tree.insert_equal(&a1) );
	EXPECT_TRUE( tree.insert_equal(&a2) );
	EXPECT_TRUE( tree.insert_equal(&a3) );
	EXPECT_TRUE( tree.insert_equal(&a4) );
	EXPECT_TRUE( tree.insert_equal(&a5) );

	tree.clear();
	EXPECT_EQ( 0, tree.size() );
	ASSERT_FALSE( a1.in_rbtree() );
	ASSERT_FALSE( a2.in_rbtree() );
	ASSERT_FALSE( a3.in_rbtree() );
	ASSERT_FALSE( a4.in_rbtree() );
	ASSERT_FALSE( a5.in_rbtree() );

	ASSERT_TRUE( a1.ref_count() == 0 );
	ASSERT_TRUE( a2.ref_count() == 0 );
	ASSERT_TRUE( a3.ref_count() == 0 );
	ASSERT_TRUE( a4.ref_count() == 0 );
	ASSERT_TRUE( a5.ref_count() == 0 );

	EXPECT_TRUE( tree.insert_equal(&a1) );
	EXPECT_TRUE( tree.insert_equal(&a2) );
	EXPECT_TRUE( tree.insert_equal(&a3) );
	EXPECT_TRUE( tree.insert_equal(&a4) );
	EXPECT_TRUE( tree.insert_equal(&a5) );

	EXPECT_FALSE( tree.insert_equal(&b2) );
	EXPECT_FALSE( b2.in_rbtree() );

	ASSERT_FALSE(tree.insert_equal(&a3));

	ebase::ref_tree::entry*	iter = tree.begin();
	ASSERT_TRUE( iter != 0 );
	EXPECT_EQ( 1,((entry_node*)iter)->value );

	iter = tree.next( iter );

	ASSERT_TRUE( iter != 0 );
	EXPECT_EQ( 2,((entry_node*)iter)->value );

	iter = tree.next( iter );

	ASSERT_TRUE( iter != 0 );
	EXPECT_EQ( 3,((entry_node*)iter)->value );

	iter = tree.next( iter );

	ASSERT_TRUE( iter != 0 );
	EXPECT_EQ( 4,((entry_node*)iter)->value );

	iter = tree.next( iter );

	ASSERT_TRUE( iter != 0 );
	EXPECT_EQ( 5,((entry_node*)iter)->value );

	iter = tree.next( iter );

	ASSERT_TRUE( iter == 0 );


	entry_node* pp = (entry_node*)tree.find((void*)1);
	ASSERT_TRUE(pp!=0);
	EXPECT_EQ(pp->value,1);

	pp = (entry_node*)tree.find((void*)-1);
	ASSERT_TRUE(pp==0);


	ASSERT_TRUE(tree.remove(&a2));

	EXPECT_EQ( 4, tree.size() );
	tree.clear();
	EXPECT_EQ( 0, tree.size() );
	ASSERT_FALSE( a1.in_rbtree() );
	ASSERT_FALSE( a2.in_rbtree() );
	ASSERT_FALSE( a3.in_rbtree() );
	ASSERT_FALSE( a4.in_rbtree() );
	ASSERT_FALSE( a5.in_rbtree() );

	ASSERT_TRUE( a1.ref_count() == 0 );
	ASSERT_TRUE( a2.ref_count() == 0 );
	ASSERT_TRUE( a3.ref_count() == 0 );
	ASSERT_TRUE( a4.ref_count() == 0 );
	ASSERT_TRUE( a5.ref_count() == 0 );

}