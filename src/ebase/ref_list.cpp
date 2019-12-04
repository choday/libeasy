#include "ref_list.hpp"
#include "macro/dblist.h"
namespace ebase
{

	ref_list::ref_list():_first(0),_last(0),_count(0)
	{

	}

	ref_list::~ref_list()
	{
		clear();
	}

	ref_list::entry* ref_list::front() const
	{
		return _first;
	}

	ref_list::entry* ref_list::back() const
	{
		return _last;
	}

	bool ref_list::push_front(entry* e)
	{
        assert(!e->in_list());
		if(e->in_list())return false;

        e->on_list_insert(this);
		DBLIST_PUSH_BACK( this,e );
		_count++;

		return true;
	}

	bool ref_list::push_back(entry* e)
	{
        assert(!e->in_list());
		if(e->in_list())return false;

        e->on_list_insert(this);
		DBLIST_PUSH_BACK( this,e );
		_count++;

		return true;
	}

    ref_list::pair ref_list::pop_front()
	{
		pair result;

		if(!DBLIST_IS_EMPTY(this))
		{
			entry* p = this->front();
			result.holder=p->get_holder();
            result.entry = p;

			DBLIST_POP_FRONT( this );
			_count--;

			p->on_list_remove(this);
		}
		return result;
	}

	ref_list::pair ref_list::pop_back()
	{
		pair result;

		if(!DBLIST_IS_EMPTY(this))
		{
			entry* p = this->back();
			result.holder=p->get_holder();
            result.entry = p;

			DBLIST_POP_BACK( this );
			_count--;

			p->on_list_remove(this);
		}
		return result;
	}

	bool ref_list::remove(entry* e)
	{
        assert(!DBLIST_IS_EMPTY(this));

        if(DBLIST_IS_EMPTY(this))return false;

        assert(e->in_list());
		if(!e || !e->in_list() )return false;
		
        assert(0==e->list_host || e->list_host==this);
		if(e->list_host!=this)return false;

		DBLIST_ERASE(this,e);
		_count--;

		e->on_list_remove(this);
		return true;
	}

	void ref_list::clear()
	{
		while(this->_first)
		{
			entry* p = this->_first;

			DBLIST_POP_FRONT(this);

			p->on_list_remove(this);
            --_count;
		}
	}


    ref_list::entry::entry()
    {
        _holder=0;
        list_host = 0;
        _prev = 0;
        _next = 0;
    }

    void ref_list::entry::on_list_insert(ref_list* list)
    {
        list_host = list;
        if(this->_holder)this->_holder->add_ref();
    }

    void ref_list::entry::on_list_remove(ref_list* list)
    {
        assert(list_host==list);
        list_host=0;
        if(this->_holder)this->_holder->release();
    }

};