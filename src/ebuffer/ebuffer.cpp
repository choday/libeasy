#include "ebuffer.hpp"
#include <stdio.h>
namespace ebuffer
{
    value_node::node_stack value_node::_node_stack = {0};

    void value_node::push_node_stack(value_node* begin,size_t class_size,value_node** node_slot,node_stack* save_stack)
    {
        *save_stack = _node_stack;
        _node_stack.frame_next = save_stack;

        value_node* end_node = (value_node*)( ((char*)begin)+class_size );

        if(0==_node_stack.stack_end)_node_stack.stack_end = end_node;
    
        _node_stack.frame_begin = begin;
        _node_stack.frame_end= end_node;
        _node_stack.next_node_slot = node_slot;
    }

    void value_node::pop_node_stack()
    {
        if(_node_stack.frame_next)
        {
            _node_stack = *_node_stack.frame_next;
        }else
        {
        }
    }

    void value_node::clear_node_stack()
    {
        _node_stack.stack_current = 0;
        _node_stack.stack_end = 0;

        _node_stack.frame_next = 0;
        _node_stack.frame_begin = 0;
        _node_stack.frame_end = 0;
        _node_stack.next_node_slot = 0;
    }

    value_node::value_node()
    {
        node_next=0;

        //«Â’ª
        if(this<=_node_stack.stack_current || this>=_node_stack.stack_end )
        {
            clear_node_stack();
        }

        //≥ˆ’ª
        if( this >= _node_stack.frame_end )
        {
            pop_node_stack();
        }

        if(_node_stack.next_node_slot)
        {
            *_node_stack.next_node_slot = this;
        }
        _node_stack.next_node_slot = &node_next;
        _node_stack.stack_current = this;
    }

    buffer_node::buffer_node(int class_size)
    {
        node_child = 0;
        push_node_stack(this,class_size,&node_child,&_stack);
    }

};