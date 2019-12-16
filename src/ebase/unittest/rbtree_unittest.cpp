#include<iostream>
#include<gtest/gtest.h>
#include <ebase/atomic.hpp>
#include <ebase/thread.hpp>

#include <set>

extern "C"
{
#include <ebase/c/rbtree.h>
}

struct test_rbentry:public rbtree_entry
{
    int             data;
};

int compare_entry(rbtree_head* head,rbtree_entry* e1,rbtree_entry* e2)
{
    test_rbentry* p1 = (test_rbentry*)e1;
    test_rbentry* p2 = (test_rbentry*)e2;

    if(p1->data<p2->data)return -1;
    if(p1->data>p2->data)return 1;
    return 0;
}
int compare_value(rbtree_head* head,rbtree_entry* e1,void* pfindvalue)
{
    test_rbentry* p1 = (test_rbentry*)e1;
    int* p2 = (int*)pfindvalue;

    if(p1->data<*p2)return -1;
    if(p1->data>*p2)return 1;
    return 0;
}

TEST(rbtree,remove)
{

}

TEST(rbtree,all)
{
    rbtree_head head={0};

    head.compare_entry = &compare_entry;
    head.compare_value = &compare_value;

    const int entry_count = 100;
    test_rbentry entry[entry_count];
    test_rbentry* p;

    //memset(entry,0,sizeof(entry) );
    for(int i=0;i<entry_count;++i)
    {
        entry[i].data = i;
        EXPECT_EQ( rbtree_insert_equal(&head,&entry[i] ),(rbtree_entry*)0);
    }

    p = (test_rbentry*)rbtree_minmax(&head,1);
    for(int i=0;i<entry_count;++i)
    {
        EXPECT_EQ(p->data,i);
        p=(test_rbentry*)rbtree_next( p );
    }
 
    test_rbentry* entryo2 = (test_rbentry*)rbtree_minmax(&head,1);
    for(int i=0;i<entry_count;++i)
    {
        ASSERT_TRUE(entryo2!=0);
        test_rbentry* entryo = (test_rbentry*)rbtree_next( entryo2 );

        EXPECT_EQ(entryo2->data,i);
        rbtree_remove(&head,entryo2);

        entryo2=entryo;
    }

    for(int i=0;i<entry_count;++i)
    {
        entry[i].data = i;
        EXPECT_EQ( rbtree_insert_equal(&head,&entry[i] ),(rbtree_entry*)0);
    }

    for(int i=0;i<entry_count;++i)
    {
        test_rbentry* entryo = (test_rbentry*)rbtree_minmax(&head,1);
        EXPECT_EQ(entryo->data,i);

        rbtree_remove(&head,entryo);
    }


    for(int i=0;i<entry_count;++i)
    {
        entry[i].data = i;
        EXPECT_EQ( rbtree_insert_equal(&head,&entry[i] ),(rbtree_entry*)0);
    }

    int find_value=1000;

    test_rbentry* findentry = (test_rbentry*)rbtree_find(&head,&find_value);
    ASSERT_TRUE(findentry==0);

    for(int i=0;i<entry_count;++i)
    {
        findentry = (test_rbentry*)rbtree_find(&head,&i);
        ASSERT_TRUE(findentry!=0 );
        if(findentry)ASSERT_EQ(findentry->data,i);
    }

    for(int i=0;i<entry_count;++i)
    {
        test_rbentry* entryo = (test_rbentry*)rbtree_minmax(&head,0);

        EXPECT_EQ(entryo->data,entry_count-i-1);

        rbtree_remove(&head,entryo);
    }


    class count_black_node
    {
    public:
        count_black_node()
        {
            memset(counter,0,sizeof(counter) );
            count_of_leaf=0;
        }

        void count(rbtree_entry* e,int number)
        {
            if(0==e)return;

            if(e->rbe_color==RB_COLOR_BLACK)number++;

            if(!e->rbe_left && !e->rbe_right )
            {
                if(count_of_leaf)ASSERT_EQ( counter[count_of_leaf-1],number );

                counter[count_of_leaf++]=number;

                return;
            }

            if(e->rbe_left)
            {
                if(e->rbe_color==RB_COLOR_RED)ASSERT_EQ(e->rbe_left->rbe_color,RB_COLOR_BLACK);
                count(e->rbe_left,number);
            }

            if(e->rbe_right)
            {
                if(e->rbe_color==RB_COLOR_RED)ASSERT_EQ(e->rbe_right->rbe_color,RB_COLOR_BLACK);
                count(e->rbe_right,number);
            }
        }


        int     count_of_leaf;
        int     counter[entry_count];
    };

    
    for(int i=0;i<entry_count;++i)
    {
        entry[i].data = i;
        EXPECT_EQ( rbtree_insert_equal(&head,&entry[i] ),(rbtree_entry*)0);
    }

    count_black_node counter_black;

    counter_black.count( head.rbh_root,1 );

    ASSERT_EQ( head.rbh_root->rbe_color,RB_COLOR_BLACK );

    for(int i=0;i<entry_count;++i)
    {
        test_rbentry* entryo = (test_rbentry*)rbtree_minmax(&head,0);

        EXPECT_EQ(entryo->data,entry_count-i-1);

        rbtree_remove(&head,entryo);
    }


    entry[0].data = 0;
    ASSERT_TRUE(0==rbtree_insert_equal(&head,&entry[0]));
    ASSERT_EQ(head.rbh_root,&entry[0]);

    entry[1].data = 0;
    ASSERT_FALSE(0==rbtree_insert_equal(&head,&entry[1]));
    ASSERT_TRUE(0==rbtree_insert_unique(&head,&entry[1]));
    
    ASSERT_EQ(entry[0].list_child,&entry[1]);
    
    ASSERT_EQ(entry[1].rbe_color,RB_COLOR_LIST);
    ASSERT_EQ(entry[1].rbe_right,&entry[1]);
    ASSERT_EQ(entry[1].rbe_left,&entry[1]);
    ASSERT_TRUE(entry[1].list_child==0);

    ASSERT_TRUE(head.rbh_root->list_child!=0);

    entry[2].data = 0;
    ASSERT_TRUE(0==rbtree_insert_unique(&head,&entry[2]));
    ASSERT_EQ(entry[2].rbe_color,RB_COLOR_LIST);
    ASSERT_TRUE(entry[1].list_child==0);
    ASSERT_TRUE(entry[2].list_child==0);

    ASSERT_EQ(entry[1].rbe_right,&entry[2]);
    ASSERT_EQ(entry[1].rbe_left,&entry[2]);
    ASSERT_EQ(entry[1].rbe_parent,&entry[0]);

    ASSERT_EQ(entry[2].rbe_right,&entry[1]);
    ASSERT_EQ(entry[2].rbe_left,&entry[1]);
    ASSERT_EQ(entry[2].rbe_parent,&entry[0]);

    
    entry[99].data = 99;
    ASSERT_TRUE(0==rbtree_insert_equal(&head,&entry[99]));


    p = (test_rbentry*)rbtree_minmax(&head,1);
    ASSERT_TRUE(p!=0);

    p = (test_rbentry*)rbtree_next(p);
    ASSERT_TRUE(p!=0);

    p = (test_rbentry*)rbtree_next(p);
    ASSERT_TRUE(p!=0);

    p = (test_rbentry*)rbtree_next(p);
    ASSERT_TRUE(p!=0);
    ASSERT_EQ( p->data,99 );
    
    p = (test_rbentry*)rbtree_next(p);
    ASSERT_TRUE(p==0);

    rbtree_remove(&head,&entry[0]);

    ASSERT_TRUE(head.rbh_root->list_child!=0);
    ASSERT_TRUE(head.rbh_root->list_child->rbe_right==head.rbh_root->list_child);
    ASSERT_TRUE(head.rbh_root->list_child->rbe_left==head.rbh_root->list_child);
    ASSERT_TRUE(head.rbh_root->list_child->list_child==0);
    ASSERT_EQ(head.rbh_root->list_child->rbe_color,RB_COLOR_LIST);

    rbtree_remove(&head,&entry[2]);

    rbtree_remove(&head,&entry[1]);
   
    rbtree_remove(&head,&entry[99]);

    ASSERT_NE(rand(),rand());

    //(rand()*10)/RAND_MAX 在android上为0,是因为RAND_MAX在linux上比较大(-1)，在win32是只有65535,需要用取模运算来限定rand范围
    for(int i=0;i<entry_count;++i)
    {
        entry[i].data = (rand()%10);

        EXPECT_EQ( rbtree_insert_unique(&head,&entry[i] ),(rbtree_entry*)0);
       
    }
    

    std::set<int> count_keys;

    for( p = (test_rbentry*)rbtree_minmax(&head,1);p!=0;p=(test_rbentry*)rbtree_next(p) )
    {
        count_keys.insert( p->data );
        
    }

    EXPECT_EQ((int)count_keys.size(),10);//android 这里count_keys.size()==1,是因为rand函数的问题
    for( p = (test_rbentry*)rbtree_minmax(&head,1);p!=0; )
    {
        test_rbentry* pnext=(test_rbentry*)rbtree_next(p);
        rbtree_remove( &head,p );
        p=pnext;
    }
}
