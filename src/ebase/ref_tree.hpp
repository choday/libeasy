#pragma once
extern "C"
{
#include "c/rbtree.h"
}
#include "ref_class.hpp"

namespace ebase
{
	//red black tree
	class ref_tree
	{
	public:
		ref_tree();
		~ref_tree();

		class entry;

		bool		        insert_equal(class entry* entry_value);
        bool		        insert_unique(class entry* entry_value);
		bool		        remove(class entry* entry_value);
		entry*		        find(void* find_value) const;
		entry*		        next(const class entry* entry_value) const;
		entry*		        begin() const;//最小值
		inline entry*       end() const{return 0;}
		entry*		        rbegin() const;//最大值
		int 		        clear();
        inline int			size() const{return _count;}

		class entry
		{
		public:
            entry();

			virtual int		    compare_rbtree_entry( entry* left_value ) = 0;
			virtual int		    compare_rbtree_find_value(void* pfind_value ) = 0;

            inline void	        set_holder(ref_class_i* holder){this->_holder=holder;}
            inline ref_class_i* get_holder(){return this->_holder;}

			template<class type>
			type*       		get_holder(){if(!_holder)return 0;return (type*)_holder;}

			inline bool		    in_rbtree() const {return this->_tree_host !=0;}
            inline entry*		rbtree_next() const{ return this->_tree_host->next(this);}
            inline bool         has_list_child() const {return 0!=this->_rbtree_entry.list_child;}
            inline entry*       list_child_first() const {return rbtree_entry2entry(this->_rbtree_entry.list_child);}


            virtual void        on_rbtree_insert(ref_tree* tree);
			virtual void	    on_rbtree_remove(ref_tree* tree);
		protected:
			
			friend class ref_tree;

            static inline entry*   rbtree_entry2entry(rbtree_entry* e){return (entry*)((char*)e - (char*)(&((entry*)0)->_rbtree_entry) );}

            ref_tree*       _tree_host;
            ref_class_i*    _holder;
			rbtree_entry    _rbtree_entry;
		};

	private:

        rbtree_head     _rbtree_head;
		int			    _count;

		static int compare_rbtree_node(rbtree_head* head, rbtree_entry* e1,rbtree_entry* e2);
		static int compare_rbtree_find_value(rbtree_head* head,void* pfind_data,rbtree_entry* e2);
	};
};