#include "ref_tree.hpp"
namespace ebase
{
    int ref_tree::compare_rbtree_node(rbtree_head* head,rbtree_entry* e1,rbtree_entry* e2)
    {
        ref_tree* pthis = (ref_tree*) ((char*)head-(char*)&((ref_tree*)0)->_rbtree_head);

        if(pthis->_entry_compare)return pthis->_entry_compare->compare_rbtree_entry(entry::rbtree_entry2entry(e1),entry::rbtree_entry2entry(e2));
        

        return entry::rbtree_entry2entry(e2)->compare_rbtree_entry( entry::rbtree_entry2entry(e1) );
    }

    int ref_tree::compare_rbtree_find_value(rbtree_head* head,void* pfind_data,rbtree_entry* e2)
    {
        ref_tree* pthis = (ref_tree*) ((char*)head-(char*)&((ref_tree*)0)->_rbtree_head);

        if(pthis->_entry_compare)return pthis->_entry_compare->compare_rbtree_find_value( pfind_data,entry::rbtree_entry2entry(e2));
        
        return entry::rbtree_entry2entry(e2)->compare_rbtree_find_value( pfind_data );
    }

    ref_tree::ref_tree():_entry_compare(0)
	{
		_rbtree_head.rbh_root = 0;
        _rbtree_head.compare_entry = &ref_tree::compare_rbtree_node;
        _rbtree_head.compare_value = &ref_tree::compare_rbtree_find_value;
		_count = 0;
	}

	ref_tree::~ref_tree()
	{
		clear();
	}

	bool ref_tree::insert_equal(class entry* entry_value)
	{
		if(entry_value->_tree_host)return false;
        
		if(0  ==  rbtree_insert_equal( &this->_rbtree_head,&entry_value->_rbtree_entry ) )
		{
            entry_value->on_rbtree_insert(this);
			_count++;
			return true;
		}

		return false;
	}

	bool ref_tree::insert_unique(class entry* entry_value)
	{
		if(entry_value->_tree_host)return false;
        
		if(0  ==  rbtree_insert_unique( &this->_rbtree_head,&entry_value->_rbtree_entry ) )
		{
			entry_value->on_rbtree_insert(this);
			_count++;
			return true;
		}

		return false;
	}

	bool ref_tree::remove(class entry* entry_value)
	{
        //if(0==this->_rbtree_head.rbh_root)return false;

        assert(this==entry_value->_tree_host);
		if(this!=entry_value->_tree_host)return false;

		rbtree_remove( &this->_rbtree_head,&entry_value->_rbtree_entry );

		entry_value->on_rbtree_remove(this);
		_count--;

		return true;
	}

	ref_tree::entry* ref_tree::find(void* pfind_value) const
	{
        rbtree_entry* p = rbtree_find( (rbtree_head*)&this->_rbtree_head,pfind_value);
        if(0==p)return 0;

        return entry::rbtree_entry2entry(p);
	}


	ref_tree::entry* ref_tree::next(const class entry* entry_value) const
	{
		if(0==entry_value)return 0;

        assert(this==entry_value->_tree_host);
		if(this!=entry_value->_tree_host)return 0;

		rbtree_entry* p = rbtree_next( (rbtree_entry*)&entry_value->_rbtree_entry );
        if(!p)return 0;

		return entry::rbtree_entry2entry(p);
	}

	ref_tree::entry* ref_tree::begin() const
	{
		rbtree_entry* p = rbtree_minmax( (rbtree_head*)&this->_rbtree_head,1 );

        if(0==p)return 0;
		return entry::rbtree_entry2entry(p);
	}

	ref_tree::entry* ref_tree::rbegin() const
	{
		rbtree_entry* p = rbtree_minmax( (rbtree_head*)&this->_rbtree_head,0 );

        if(0==p)return 0;
		return entry::rbtree_entry2entry(p);
	}

	int ref_tree::clear()
	{

        class clear_callback
        {
        public:
            static inline void callback(rbtree_entry *entry,void* userdata)
            {
                ref_tree* pthis = (ref_tree*)userdata;
                pthis->_count --;
                entry::rbtree_entry2entry(entry)->on_rbtree_remove(pthis);
            }
        };

        return rbtree_clear( &this->_rbtree_head,&clear_callback::callback,this ); 
	}

 
    void ref_tree::set_entry_compare(entry_compare* c)
    {
        _entry_compare = c;
    }

    ref_tree::entry::entry()
    {
        _tree_host=0;
        _holder = 0;
    }


    void ref_tree::entry::on_rbtree_insert(ref_tree* tree)
    {
        this->_tree_host=tree;
        if(this->_holder)this->_holder->add_ref();
    }

    void ref_tree::entry::on_rbtree_remove(ref_tree* tree)
	{
        this->_tree_host=0;
        if(this->_holder)this->_holder->release();
	}


};