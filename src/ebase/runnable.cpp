#include "runnable.hpp"
#include "macro/dblist.h"
namespace ebase
{

	runnable::runnable(ref_class_i* outref /*= nullptr*/):ref_class(outref)
	{
		_prev = 0;
	}

	void runnable::on_abort()
	{

	}

	bool runnable::is_schedule()
	{
		return DBLIST_IS_IN_LIST(this);
	}

	runnable_queue::runnable_queue()
	{
		_last = 0;
		_count = 0;
	}

	runnable_queue::~runnable_queue()
	{
		clear();
	}

	ebase::ref_ptr<runnable> runnable_queue::front()
	{
		return this->_first;
	}

	bool runnable_queue::push_back(runnable* request)
	{
		if(!request || request->is_schedule())return false;

		assert(request->ref_count()>0);

		{
			DBLIST_PUSH_BACK( this,request );
			_count++;
		}

		return true;
	}

	ref_ptr<runnable> runnable_queue::pop_front()
	{
		ebase::ref_ptr<runnable> result=this->_first;

		if(!DBLIST_IS_EMPTY(this))
		{
			DBLIST_POP_FRONT( this );
			_count--;
		}
		return result;
	}

	bool runnable_queue::remove(runnable* request)
	{
		if(!request || !request->is_schedule())return false;

		if(!DBLIST_IS_EMPTY(this))
		{
			DBLIST_ERASE(this,request);
			_count--;
		}
		return true;
	}

	void runnable_queue::clear()
	{
		while(this->_first)
		{
			DBLIST_POP_FRONT(this);
		}
		_count=0;
	}

	int runnable_queue::size() const
	{
		return _count;
	}

};