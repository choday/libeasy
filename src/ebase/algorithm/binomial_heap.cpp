#include "binomial_heap.hpp"


namespace ebase
{
    namespace algorithm
    {
        void                    __binomial_uncache_min(struct binomial_head* heap );
        void                    __binomial_insert_child(struct binomial_node* node,struct binomial_node* child);
        struct binomial_node*   __binomial_merge(struct binomial_node* a,struct binomial_node* b);
        struct binomial_node*   __binomial_reverse(struct binomial_node* h);
        void                    __binomial_min(struct binomial_head* heap,struct binomial_node** prev, struct binomial_node** node );
        void                    __binomial_union( struct binomial_head* heap,struct binomial_node* h2);
        struct binomial_node*   __binomial_extract_min(struct binomial_head* heap);
        void                    __binomial_swap_parent(struct binomial_head* heap,struct binomial_node* node,bool condition_swap );//condition_swap=false无条件交换，直到最顶层

        /* insert (and reinitialize) a node into the heap */
        void binomial_push(struct binomial_head* heap,struct binomial_node* node)
        {
	        struct binomial_node *min;
	        node->child  = 0;
	        node->parent = 0;
	        node->next   = 0;
	        node->child_count = 0;
	        if (heap->min && heap->less_proc(node, heap->min)) {
		        /* swap min cache */
		        min = heap->min;
		        min->child  = 0;
		        min->parent = 0;
		        min->next   = 0;
		        min->child_count = 0;
		        __binomial_union(heap, min);
		        heap->min   = node;
	        } else
		        __binomial_union(heap, node);
        }

        /* merge addition into target */
        void binomial_union(struct binomial_head* target, struct binomial_head* addition)
        {
	        /* first insert any cached minima, if necessary */
	        __binomial_uncache_min(target);
	        __binomial_uncache_min(addition);
	        __binomial_union(target, addition->head);
	        /* this is a destructive merge */
	        addition->head = 0;
        }

        struct binomial_node* heap_get_first(struct binomial_head* heap )
        {
	        if (!heap->min)
		        heap->min = __binomial_extract_min(heap);
	        return heap->min;
        }

        struct binomial_node* heap_pop_first(struct binomial_head* heap )
        {
	        struct binomial_node *node;
	        if (!heap->min)
		        heap->min = __binomial_extract_min(heap);
	        node = heap->min;
	        heap->min = 0;
	        if (node)node->child_count = -1;
	        return node;
        }

        void binomial_increase(struct binomial_head* heap,struct binomial_node* node)
        {

        }

        void binomial_decrease(struct binomial_head* heap,struct binomial_node* node)
        {

        }

        //front与node同级，从front往后查找，找到node的前一个
        inline struct binomial_node* __binomial_find_prev(struct binomial_node* front,struct binomial_node* node)
        {
            if(front==node)return 0;

            while(front && front->next!=node)
            {
                front=front->next;
            }

            return front;
        }

        void binomial_remove(struct binomial_head* heap,struct binomial_node* node)
        {
	        if (heap->min == node)
            {
		        heap->min = 0;
	            node->child_count = -1;
                return;
            }

            __binomial_swap_parent(heap,node,false);

	        struct binomial_node *prev;

		    prev = __binomial_find_prev( heap->head,node );
            if( 0 == prev )
            {
                heap->head = node->next;
            }else 
            {
                prev = prev->next;
            }

            node->next=0;

            __binomial_union(heap, __binomial_reverse(node->child));
        }

        //交换子母节点,注意,node->childs在交换以后没有全部更新为新的parent,请调用__binomial_update_childs()更新
        void __binomial_swap_node(struct binomial_head* heap,struct binomial_node* parent,struct binomial_node* node)
        {
            struct binomial_node temp = *parent;

            struct binomial_node* node_prev = __binomial_find_prev(parent->child,node);
            struct binomial_node* parent_prev = 0;
            
            if(parent->parent)
            {
                parent_prev = __binomial_find_prev(parent->parent->child,parent);
            }else
            {
                parent_prev = __binomial_find_prev(heap->head,parent);
            }
 
            parent->child_count = node->child_count;
            parent->child = node->child;
            parent->next = node->next;
            parent->parent = node;
    
            
            node->child_count = temp.child_count;
            node->next = temp.next;
            node->parent = temp.parent;  
            if(node_prev)node_prev->next = parent;

            if(temp.child==node)node->child = parent;
            else node->child = temp.child;

            if(temp.parent)
            {
                if( temp.parent->child == parent)temp.parent->child = node;
                else parent_prev = __binomial_find_prev(parent->parent->child,parent);
            }else
            {

            }

            
            if(parent_prev)parent_prev->next = node;
        }

        //更新node人全部子节点 的parent=node
        void __binomial_update_childs(struct binomial_node* node)
        {
            struct binomial_node* child=node->child;
            while(child)
            {
                child->parent = node;
                child=child->next;
            }
        }

        //node与parent交换
        void __binomial_swap_parent(struct binomial_head* heap,struct binomial_node* node,bool condition_swap )
        {
            struct binomial_node* parent = node->parent;

            while(parent)
            {
                if( condition_swap && !heap->less_proc(node,parent) )break;//if parent <= node break

                __binomial_swap_node(parent,node);
                __binomial_update_childs(parent);                

                if( heap->head == parent )heap->head=node;
                parent = node->parent;
            }

            __binomial_update_childs(node);
        }

        void __binomial_swap_children(struct binomial_head* heap,struct binomial_node* root)
        {

        }

        void __binomial_uncache_min(struct binomial_head* heap )
        {
	        struct binomial_node* min;
            
	        if (heap->min) {
		        min = heap->min;
		        heap->min = 0;
		        binomial_push(heap, min);
	        }
        }

        void __binomial_insert_child(struct binomial_node* node,struct binomial_node* child)
        {
	        child->parent = node;
	        child->next   = node->child;
	        node->child   = child;
	        node->child_count++;
        }

        /* merge root lists */
        struct binomial_node* __binomial_merge(struct binomial_node* a,struct binomial_node* b)
        {
	        struct binomial_node* head = 0;
	        struct binomial_node** pos = &head;

	        while (a && b) {
		        if (a->child_count < b->child_count) {
			        *pos = a;
			        a = a->next;
		        } else {
			        *pos = b;
			        b = b->next;
		        }
		        pos = &(*pos)->next;
	        }
	        if (a)
		        *pos = a;
	        else
		        *pos = b;
	        return head;
        }

        /* reverse a linked list of nodes. also clears parent pointer */
        struct binomial_node* __binomial_reverse(struct binomial_node* h)
        {
	        struct binomial_node* tail = 0;
	        struct binomial_node* next;

	        if (!h)
		        return h;

	        h->parent = 0;
	        while (h->next) {
		        next    = h->next;
		        h->next = tail;
		        tail    = h;
		        h       = next;
		        h->parent = 0;
	        }
	        h->next = tail;
	        return h;
        }

        void __binomial_min(struct binomial_head* heap,struct binomial_node** prev, struct binomial_node** node )
        {
	        struct binomial_node *_prev, *cur;
	        *prev = 0;

	        if (!heap->head) {
		        *node = 0;
		        return;
	        }

	        *node = heap->head;
	        _prev = heap->head;
	        cur   = heap->head->next;
	        while (cur) {
		        if ( heap->less_proc(cur, *node)) {
			        *node = cur;
			        *prev = _prev;
		        }
		        _prev = cur;
		        cur   = cur->next;
	        }
        }

                
        void __binomial_union( struct binomial_head* heap,struct binomial_node* h2)
        {
	        struct binomial_node* h1;
	        struct binomial_node *prev, *x, *next;
	        if (!h2)
		        return;
	        h1 = heap->head;
	        if (!h1) {
		        heap->head = h2;
		        return;
	        }
	        h1 = __binomial_merge(h1, h2);
	        prev = 0;
	        x    = h1;
	        next = x->next;
	        while (next) {
		        if (x->child_count != next->child_count ||
		            (next->next && next->next->child_count == x->child_count)) {
			        /* nothing to do, advance */
			        prev = x;
			        x    = next;
		        } else if ( heap->less_proc(x, next)) {
			        /* x becomes the root of next */
			        x->next = next->next;
			        __binomial_insert_child(x, next);
		        } else {
			        /* next becomes the root of x */
			        if (prev)
				        prev->next = next;
			        else
				        h1 = next;
			        __binomial_insert_child(next, x);
			        x = next;
		        }
		        next = x->next;
	        }
	        heap->head = h1;
        }


        struct binomial_node* __binomial_extract_min(struct binomial_head* heap)
        {
	        struct binomial_node *prev, *node;
	        __binomial_min( heap, &prev, &node);
	        if (!node)
		        return 0;
	        if (prev)
		        prev->next = node->next;
	        else
		        heap->head = node->next;
	        __binomial_union(heap, __binomial_reverse(node->child));
	        return node;
        }

    };
};