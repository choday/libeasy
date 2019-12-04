#ifndef _RB_TREE_H_
#define _RB_TREE_H_

#define RB_COLOR_LIST       0 //circle list
#define RB_COLOR_BLACK	    -1
#define RB_COLOR_RED        1

typedef struct _rbtree_entry_tiny_
{
    struct _rbtree_entry_tiny_*  rbe_left;//list.prev
    struct _rbtree_entry_tiny_*  rbe_right;//list.next
    struct _rbtree_entry_tiny_*  rbe_parent;
    int                          rbe_color;
} rbtree_entry_tiny;

typedef struct _rbtree_entry_
{
    struct _rbtree_entry_*  rbe_left;//list.prev
    struct _rbtree_entry_*  rbe_right;//list.next
    struct _rbtree_entry_*  rbe_parent;
    int                     rbe_color;
    struct _rbtree_entry_*  list_child;//circle list
} rbtree_entry;

typedef struct _rbtree_head_
{
    struct _rbtree_entry_* rbh_root;

    int     (*compare_entry)(struct _rbtree_head_* head,rbtree_entry* e1,rbtree_entry* e2);
    int     (*compare_value)(struct _rbtree_head_* head,void* pfindvalue,rbtree_entry* e2);
} rbtree_head;

//注意删除优化,尽量先删除list中的节点，最后再删除tree 节点,即while(elm->list_child)rbtree_remove(head,elm->list_child);

void                    rbtree_remove(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm);
struct _rbtree_entry_ * rbtree_insert_equal(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm);//成功返回0
struct _rbtree_entry_ * rbtree_insert_unique(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm);//成功返回0
struct _rbtree_entry_ * rbtree_find(struct _rbtree_head_ *head, void* pfindvalue );
struct _rbtree_entry_ * rbtree_next(struct _rbtree_entry_ *elm);
struct _rbtree_entry_ * rbtree_minmax(struct _rbtree_head_ *head, int return_min);
int                     rbtree_clear(struct _rbtree_head_ *head,void (*release_entry)(rbtree_entry *,void* userdata),void* userdata );


//以下xxx_tiny函数，无list_child成员,对应的结构为rbtree_entry_tiny
struct _rbtree_entry_ * rbtree_insert_tiny(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm);//成功返回0
void                    rbtree_remove_tiny(struct _rbtree_head_ *head, struct _rbtree_entry_ *elm);
struct _rbtree_entry_ * rbtree_next_tiny(struct _rbtree_entry_ *elm);
int                     rbtree_clear_tiny(struct _rbtree_head_ *head,void (*release_entry)(rbtree_entry_tiny *,void* userdata),void* userdata );

#endif //_RB_TREE_H_