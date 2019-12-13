#pragma once
#include "../ebase/precompile.h"
namespace ebuffer
{
    //构造函数调用顺序，父类->成员变量->this

    class value_node
    {
    public:
        value_node();

        value_node*     node_next;

    protected:
        struct node_stack
        {
            //栈范围，stack_current-stack-end,如果超出范围，需要清栈
            value_node*     stack_current;//当前栈,只能加，不能减
            value_node*     stack_end;//结束栈

            //栈帧，超出栈帧需要pop
            node_stack*     frame_next;
            value_node*     frame_begin;
            value_node*     frame_end;

            value_node*     *next_node_slot;
        };
        static __thread node_stack _node_stack;

        void push_node_stack( value_node* begin,size_t class_size,value_node** node_slot,node_stack* save_stack );
        void pop_node_stack();
        void clear_node_stack();
    };

    class buffer_node:public value_node
    {
    public:
        buffer_node(int class_size);

        value_node*         node_child;

    private:
        value_node::node_stack     _stack;
    };

    template<typename CLASS_NAME>
    class ebuffer_t:public buffer_node
    {
    public:
        ebuffer_t():buffer_node(sizeof(CLASS_NAME))
        {

        }
    };

    template<class ebuffer_class>
    class ebuffer_write
    {
    public:


    };
};