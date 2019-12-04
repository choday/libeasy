#include "rbtree.h"
#include <assert.h>
#ifndef NULL
#define NULL 0
#endif

//RB_GENERATE(_rbtree_head_,_rbtree_entry_,data,compare_node);

rbtree_entry*   __rbtree_rotate_left(rbtree_head* head,rbtree_entry* elm);
rbtree_entry*   __rbtree_rotate_right(rbtree_head* head,rbtree_entry* elm);
void            __rbtree_insert_color(rbtree_head* head,rbtree_entry* elm);
void            __rbtree_remove_color(struct _rbtree_head_ *head, struct _rbtree_entry_ *parent, struct _rbtree_entry_ *elm);
struct _rbtree_entry_*  __rbtree_circle_list_insert(struct _rbtree_entry_* parent,struct _rbtree_entry_ *afterthis,struct _rbtree_entry_ *elm);//return afterthis or elm;;
struct _rbtree_entry_ * __rbtree_circle_list_remove(struct _rbtree_entry_* elm);


struct _rbtree_entry_ * rbtree_find_by_entry(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm) 
{ 
    struct _rbtree_entry_ *tmp = head->rbh_root; 
    int comp; 
    assert(head->compare_entry);
    while (tmp) 
    {
        comp = head->compare_entry(head,elm, tmp); 
        if (comp < 0) tmp = tmp->rbe_left; 
        else if (comp > 0) tmp = tmp->rbe_right; 
        else return (tmp); 
    } 

    return (NULL); 
} 


struct _rbtree_entry_ * rbtree_find(struct _rbtree_head_ *head, void* pfindvalue)
{
    struct _rbtree_entry_ *tmp = head->rbh_root; 
    int comp; 
    assert(head->compare_value);
    while (tmp) 
    {
        comp = head->compare_value( head,pfindvalue, tmp); 
        if (comp < 0) tmp = tmp->rbe_left; 
        else if (comp > 0) tmp = tmp->rbe_right; 
        else return (tmp); 
    } 

    return 0;
}

struct _rbtree_entry_ * rbtree_next(struct _rbtree_entry_ *elm) 
{
    //circle list begin
    if(elm->list_child)return elm->list_child;

    if(elm->rbe_color==RB_COLOR_LIST)
    {
        //不是最后一个
        if( elm->rbe_right != elm->rbe_parent->list_child )return elm->rbe_right;

        elm = elm->rbe_parent;
    }
    //circle list end

    return rbtree_next_tiny(elm);
}

struct _rbtree_entry_ * rbtree_next_tiny(struct _rbtree_entry_ *elm)
{
    if (elm->rbe_right) 
    { 
        elm = elm->rbe_right; 
        
        while (elm->rbe_left) elm = elm->rbe_left; 
    } else 
    {
        if (elm->rbe_parent && (elm == elm->rbe_parent->rbe_left)) 
        {
            elm = elm->rbe_parent; 
        }else 
        {
            while (elm->rbe_parent && (elm == elm->rbe_parent->rbe_right)) elm = elm->rbe_parent; 
            
            elm = elm->rbe_parent; 
        } 
    }
    return elm; 
}

struct _rbtree_entry_ * rbtree_minmax(struct _rbtree_head_ *head, int return_min) 
{ 
    struct _rbtree_entry_ *tmp = head->rbh_root; 
    struct _rbtree_entry_ *parent = NULL; 
    
    while (tmp) 
    { 
        parent = tmp; 
        if (return_min) tmp = tmp->rbe_left; 
        else tmp = tmp->rbe_right; 
    } 
    
    return (parent); 
};

int rbtree_clear_tiny(struct _rbtree_head_ *head,void (*release_entry)(rbtree_entry_tiny *,void* userdata),void* userdata )
{
   int count = 0;

    rbtree_entry*   parent = head->rbh_root;
	rbtree_entry*   node;

    
    while(parent)
    {
        //从node出发，找出任意一个叶节点，如果node下面无节点，返回node
        node=parent;
        while(node)
        {
	        if(node->rbe_left)node = node->rbe_left;
	        else if(node->rbe_right)node = node->rbe_right;
	        else break;
        }

        parent=node->rbe_parent;
        if(parent)
        {
            if(node == parent->rbe_left)parent->rbe_left = 0;
            else if(node == parent->rbe_right)parent->rbe_right = 0;
            else assert(0);
        }
/*
        while(node->list_child)
        {
            count++;
            release_entry(node->list_child,userdata);
            node->list_child = __rbtree_circle_list_remove(node->list_child);
        }*/

        count++;
        release_entry((rbtree_entry_tiny*)node,userdata);
        node = 0;
    }

    head->rbh_root = 0;
    return count;
}

int rbtree_clear(struct _rbtree_head_ *head,void (*release_entry)(rbtree_entry *,void* userdata),void* userdata )
{
    int count = 0;

    rbtree_entry*   parent = head->rbh_root;
	rbtree_entry*   node;

    
    while(parent)
    {
        //从node出发，找出任意一个叶节点，如果node下面无节点，返回node
        node=parent;
        while(node)
        {
	        if(node->rbe_left)node = node->rbe_left;
	        else if(node->rbe_right)node = node->rbe_right;
	        else break;
        }

        parent=node->rbe_parent;
        if(parent)
        {
            if(node == parent->rbe_left)parent->rbe_left = 0;
            else if(node == parent->rbe_right)parent->rbe_right = 0;
            else assert(0);
        }

        while(node->list_child)
        {
            count++;
            release_entry(node->list_child,userdata);
            node->list_child = __rbtree_circle_list_remove(node->list_child);
        }

        count++;
        release_entry(node,userdata);
        node = 0;
    }

    head->rbh_root = 0;
    return count;
}

void rbtree_remove(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm) 
{

    //circle list begin

    if(elm->rbe_color==RB_COLOR_LIST)
    {
        if(elm->rbe_right == elm->rbe_parent->list_child )//最后一个
        {
            elm->rbe_parent->list_child=0;
        }else
        {
            elm->rbe_parent->list_child=__rbtree_circle_list_remove(elm);
        }
        return;
    }else if(elm->list_child)
    {
        //需要用list_child去替换elm
        struct _rbtree_entry_* list_child_save = __rbtree_circle_list_remove(elm->list_child);
        struct _rbtree_entry_* tree_node_new = elm->list_child;
        
        *tree_node_new = *elm;
        tree_node_new->list_child = list_child_save;

        if( tree_node_new->rbe_left)tree_node_new->rbe_left->rbe_parent = tree_node_new;
        if( tree_node_new->rbe_right)tree_node_new->rbe_right->rbe_parent = tree_node_new;

        if(tree_node_new->rbe_parent)
        {
            if(tree_node_new->rbe_parent->rbe_left == elm)tree_node_new->rbe_parent->rbe_left = tree_node_new;
            if(tree_node_new->rbe_parent->rbe_right == elm)tree_node_new->rbe_parent->rbe_right = tree_node_new;
        }else
        {
            assert(head->rbh_root==elm);
            head->rbh_root=tree_node_new;
        }

        if(list_child_save)list_child_save->rbe_parent = tree_node_new;

        while(list_child_save && list_child_save->rbe_right != tree_node_new->list_child )
        {
            list_child_save->rbe_parent = tree_node_new;
            list_child_save=list_child_save->rbe_right;
        }

        return;
    }else
    {
        rbtree_remove_tiny(head,elm);
    }
}
void rbtree_remove_tiny(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm)
{
    struct _rbtree_entry_ *child, *parent, *old = elm; 
    int color; 


    if (elm->rbe_left == NULL) child = elm->rbe_right; 
    else if (elm->rbe_right == NULL) child = elm->rbe_left; 
    else 
    {
        struct _rbtree_entry_ *left; 
        
        elm = elm->rbe_right;
        

        //while (left = elm->rbe_left) elm = left; 
        //modify as below
        left = elm->rbe_left;
        while(left)
        {
            elm = left;
            left = elm->rbe_left;
        }
        
        
        child = elm->rbe_right; parent = elm->rbe_parent; color = elm->rbe_color; 
        
        if (child) child->rbe_parent = parent; 
        
        if (parent) 
        { 
            if (parent->rbe_left == elm) parent->rbe_left = child; 
            else parent->rbe_right = child; 
            
        } else 
        {
            head->rbh_root = child; 
        }
        
        if (elm->rbe_parent == old) parent = elm; 
        
        *elm = *old; 
        
        if (old->rbe_parent) 
        { 
            if (old->rbe_parent->rbe_left == old) old->rbe_parent->rbe_left = elm; 
            else old->rbe_parent->rbe_right = elm; 
            
        } else 
        {
            head->rbh_root = elm; 
        }
        
        old->rbe_left->rbe_parent = elm; 
        
        if (old->rbe_right) old->rbe_right->rbe_parent = elm; 
        
        if (parent) 
        { 
            left = parent; 
            
            while(left){left = left->rbe_parent;}; 
        } 
        
        goto color; 
    } 
    
    parent = elm->rbe_parent; 
    color = elm->rbe_color; 
    if (child) child->rbe_parent = parent; 
    
    if (parent) 
    { 
        if (parent->rbe_left == elm) parent->rbe_left = child; 
        else parent->rbe_right = child; 
    } else 
        head->rbh_root = child; 

color: 
    if (color == RB_COLOR_BLACK) __rbtree_remove_color(head, parent, child); 
    //return (old); 
} 

struct _rbtree_entry_ * rbtree_insert_unique(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm)
{
    
    struct _rbtree_entry_ * p = rbtree_insert_equal( head,elm );

    elm->list_child = 0;

    if(p)p->list_child = __rbtree_circle_list_insert(p,p->list_child,elm);

    return 0;
}

struct _rbtree_entry_ * rbtree_insert_equal(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm)
{ 
       elm->list_child = 0;
       return rbtree_insert_tiny(head,elm);
}
struct _rbtree_entry_ * rbtree_insert_tiny(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm)
{
    struct _rbtree_entry_ *tmp; 
    struct _rbtree_entry_ *parent = NULL; 
    int comp = 0; 
    
    //find
    tmp = head->rbh_root; 
    while (tmp) 
    { 
        parent = tmp; 
        comp = head->compare_entry(head,elm, parent); 

        if (comp < 0) tmp = tmp->rbe_left; 
        else if (comp > 0) tmp = tmp->rbe_right; 
        else return tmp; 
    } 
    

    do { elm->rbe_parent = parent; elm->rbe_left = elm->rbe_right = NULL; elm->rbe_color = RB_COLOR_RED; } while (0); 
    
    if (parent != NULL) 
    { 
        if (comp < 0) parent->rbe_left = elm; 
        else parent->rbe_right = elm; 
        
    } else 
        head->rbh_root = elm; 
    
    __rbtree_insert_color(head, elm); 
    return 0;
} 

rbtree_entry* __rbtree_rotate_left(rbtree_head* head,rbtree_entry* elm)
{

    rbtree_entry* tmp;

    tmp = elm->rbe_right; 
    elm->rbe_right = tmp->rbe_left;
    if (tmp->rbe_left)tmp->rbe_left->rbe_parent = elm; 

    
    tmp->rbe_parent = elm->rbe_parent;
    if (elm->rbe_parent) 
    { 
        if (elm == elm->rbe_parent->rbe_left) elm->rbe_parent->rbe_left = tmp; 
        else elm->rbe_parent->rbe_right = tmp; 
    }else 
    {
        head->rbh_root = tmp;
    }
    
    tmp->rbe_left = elm; 
    elm->rbe_parent = tmp;
    


    return tmp;
}



rbtree_entry* __rbtree_rotate_right(rbtree_head* head,rbtree_entry* elm)
{
    rbtree_entry* tmp;

    tmp = elm->rbe_left; 
    
    elm->rbe_left = tmp->rbe_right;
    if (tmp->rbe_right)tmp->rbe_right->rbe_parent = elm; 
 

    
    tmp->rbe_parent = elm->rbe_parent;
    if (elm->rbe_parent)
    { 
        if (elm == elm->rbe_parent->rbe_left) elm->rbe_parent->rbe_left = tmp; 
        else elm->rbe_parent->rbe_right = tmp; 

    }else 
    {
        head->rbh_root = tmp; 
    }
    
    tmp->rbe_right = elm; 
    elm->rbe_parent = tmp;

    return tmp;
}

void __rbtree_insert_color(rbtree_head* head,rbtree_entry* elm) 
{ 
    struct _rbtree_entry_ *parent, *gparent, *tmp; 

    while ((parent = elm->rbe_parent) && parent->rbe_color == RB_COLOR_RED) 
    { 
        gparent = parent->rbe_parent; 
        if (parent == gparent->rbe_left) 
        { 
            tmp = gparent->rbe_right; 
            if (tmp && tmp->rbe_color == RB_COLOR_RED) 
            { 
                tmp->rbe_color = RB_COLOR_BLACK; 
                
                do { parent->rbe_color = RB_COLOR_BLACK; gparent->rbe_color = RB_COLOR_RED; } while (0); 

                elm = gparent; 
                continue; 
            } 
            
            if (parent->rbe_right == elm) 
            { 
                tmp = __rbtree_rotate_left(head, parent); 
                tmp = parent; 
                parent = elm; 
                elm = tmp; 
            } 
            
            do { parent->rbe_color = RB_COLOR_BLACK; gparent->rbe_color = RB_COLOR_RED; } while (0); 
            
            tmp = __rbtree_rotate_right(head, gparent ); 
        } else 
        { 
            tmp = gparent->rbe_left; 
            if (tmp && tmp->rbe_color == RB_COLOR_RED) 
            { 
                tmp->rbe_color = RB_COLOR_BLACK; 
                do { parent->rbe_color = RB_COLOR_BLACK; gparent->rbe_color = RB_COLOR_RED; } while (0); 
                elm = gparent; 
                continue; 
            } 
            if (parent->rbe_left == elm) 
            { 
                tmp = __rbtree_rotate_right(head, parent ); 
                tmp = parent; parent = elm; elm = tmp; 
            } 
            
            do { parent->rbe_color = RB_COLOR_BLACK; gparent->rbe_color = RB_COLOR_RED; } while (0); 
            tmp = __rbtree_rotate_left(head, gparent); 
        } 
    } 
    head->rbh_root->rbe_color = RB_COLOR_BLACK; 
} 

void __rbtree_remove_color(struct _rbtree_head_ *head, struct _rbtree_entry_ *parent, struct _rbtree_entry_ *elm)
{ 
    struct _rbtree_entry_ *tmp; 

    while ((elm == NULL || elm->rbe_color == RB_COLOR_BLACK) && elm != head->rbh_root) 
    { 
        if (parent->rbe_left == elm) 
        { 
            tmp = parent->rbe_right; 
            
            if (tmp->rbe_color == RB_COLOR_RED) 
            { 
                do { tmp->rbe_color = RB_COLOR_BLACK; parent->rbe_color = RB_COLOR_RED; } while (0); 
                
                tmp = __rbtree_rotate_left(head, parent); 
                tmp = parent->rbe_right; 
            } 
            
            if ((tmp->rbe_left == NULL || tmp->rbe_left->rbe_color == RB_COLOR_BLACK) 
                && (tmp->rbe_right == NULL || tmp->rbe_right->rbe_color == RB_COLOR_BLACK)) 
            { 
                tmp->rbe_color = RB_COLOR_RED; elm = parent; parent = elm->rbe_parent; 
            } else 
            { 
                if (tmp->rbe_right == NULL || tmp->rbe_right->rbe_color == RB_COLOR_BLACK) 
                { 
                    struct _rbtree_entry_ *oleft; 
                    if ((oleft = tmp->rbe_left)) oleft->rbe_color = RB_COLOR_BLACK; 
                    
                    tmp->rbe_color = RB_COLOR_RED; 
                    oleft = __rbtree_rotate_right(head, tmp); 
                    tmp = parent->rbe_right; 
                } 
                tmp->rbe_color = parent->rbe_color; 
                parent->rbe_color = RB_COLOR_BLACK; 
                if (tmp->rbe_right) tmp->rbe_right->rbe_color = RB_COLOR_BLACK; 
                
                tmp=__rbtree_rotate_left(head, parent); 
                elm = head->rbh_root; 
                break; 
            } 
        } else 
        { 
            tmp = parent->rbe_left; 
            if (tmp->rbe_color == RB_COLOR_RED) 
            { 
                do { tmp->rbe_color = RB_COLOR_BLACK; parent->rbe_color = RB_COLOR_RED; } while (0);
                tmp=__rbtree_rotate_right(head, parent); 
                tmp = parent->rbe_left; 
            } 
            
            if ((tmp->rbe_left == NULL || tmp->rbe_left->rbe_color == RB_COLOR_BLACK) 
                && (tmp->rbe_right == NULL || tmp->rbe_right->rbe_color == RB_COLOR_BLACK)) 
            { 
                tmp->rbe_color = RB_COLOR_RED; elm = parent; parent = elm->rbe_parent; 
            } else 
            { 
                if (tmp->rbe_left == NULL || tmp->rbe_left->rbe_color == RB_COLOR_BLACK) 
                { 
                    struct _rbtree_entry_ *oright; 
                    if ((oright = tmp->rbe_right)) oright->rbe_color = RB_COLOR_BLACK; 
                    
                    tmp->rbe_color = RB_COLOR_RED; 
                    oright=__rbtree_rotate_left(head, tmp); 
                    tmp = parent->rbe_left; 
                } 
                tmp->rbe_color = parent->rbe_color; 
                parent->rbe_color = RB_COLOR_BLACK; 
                if (tmp->rbe_left) tmp->rbe_left->rbe_color = RB_COLOR_BLACK; 
                
                tmp=__rbtree_rotate_right(head, parent); 
                elm = head->rbh_root; 
                break; 
            } 
        } 
    } 
    
    if (elm) elm->rbe_color = RB_COLOR_BLACK; 

} 


struct _rbtree_entry_* __rbtree_circle_list_remove(struct _rbtree_entry_* elm)
{
    struct _rbtree_entry_* result = elm->rbe_right;
    
    if(result==elm)return 0;

    elm->rbe_right->rbe_left = elm->rbe_left;
    elm->rbe_left->rbe_right = elm->rbe_right;

    return result;
}

struct _rbtree_entry_ * __rbtree_circle_list_insert(struct _rbtree_entry_* parent,struct _rbtree_entry_ *afterthis,struct _rbtree_entry_ *elm)//return afterthis or elm;
{
    elm->rbe_color = RB_COLOR_LIST;
    elm->rbe_parent = parent;

    if(0==afterthis)
    {
        elm->rbe_left = elm->rbe_right = elm;
        return elm;
    }else
    {
        elm->rbe_right = (afterthis->rbe_right);
        (afterthis->rbe_right)->rbe_left = elm;

        afterthis->rbe_right = elm;
        elm->rbe_left = afterthis;
    }

    return afterthis;
}