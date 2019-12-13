#pragma once
#include "../ebase/precompile.h"
namespace ebuffer
{
    //���캯������˳�򣬸���->��Ա����->this

    class value_node
    {
    public:
        value_node();

        value_node*     node_next;

    protected:
        struct node_stack
        {
            //ջ��Χ��stack_current-stack-end,���������Χ����Ҫ��ջ
            value_node*     stack_current;//��ǰջ,ֻ�ܼӣ����ܼ�
            value_node*     stack_end;//����ջ

            //ջ֡������ջ֡��Ҫpop
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