#pragma once

namespace ebase
{
    namespace algorithm
    {

        struct binomial_node
        {
	        struct binomial_node* 	parent;
            struct binomial_node* 	next;
	        struct binomial_node* 	child;
	        int 		            child_count;
        };

        /* item comparison function:
         * return 1 if a has less key than b, 0 otherwise
         */
        typedef int (*lpfn_binomial_less)(struct binomial_node* a, struct binomial_node* b);

        struct binomial_head {
	        struct binomial_node* 	head;
	        struct binomial_node*	min;
            lpfn_binomial_less      less_proc;
        };


        struct binomial_node*   heap_get_first(struct binomial_head* heap );
        struct binomial_node*   heap_pop_first(struct binomial_head* heap );
        void                    binomial_push(struct binomial_head* heap,struct binomial_node* node);
        void                    binomial_increase(struct binomial_head* heap,struct binomial_node* node);
        void                    binomial_decrease(struct binomial_head* heap,struct binomial_node* node);
        void                    binomial_remove(struct binomial_head* heap,struct binomial_node* node);
        void                    binomial_union(struct binomial_head* target, struct binomial_head* addition);

        static inline void binomial_init_heap(struct binomial_head* heap,lpfn_binomial_less less_proc)
        {
	        heap->head    = 0;
	        heap->min     = 0;
            heap->less_proc = less_proc;
        }

        static inline void binomial_init_node(struct binomial_node* node)
        {
	        node->parent       = 0;
            node->next         = 0;
            node->child        = 0;
            node->child_count  = -1;
        }

        static inline bool binomial_node_in_heap(struct binomial_node* node)
        {
            return node->child_count == -1;
        }

        static inline bool binomial_heap_empty(struct binomial_head* heap)
        {
	        return heap->head == 0 && heap->min == 0;
        }

    };
};