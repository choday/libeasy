#include "event_emitter.hpp"

namespace ebase
{

	event_emitter::event_emitter():_has_event(false),_event_source(0),_event_executor(0)
	{

	}

	event_emitter::~event_emitter()
	{
		assert(!this->is_schedule());
		clear();
	}

	void event_emitter::clear()
	{
        _function.clear();
		_event_source = 0;
		_event_executor = 0;
        _has_event=false;
	}


	bool event_emitter::fire()
	{
        _has_event=true;
		if(!_function.isset())
        {
            
            return false;
        }

		if(this->is_schedule())return true;
        
		if( 0==_event_executor )
		{
            _has_event=false;
			this->run();
			return true;
		}
		return _event_executor->post(this);
	}

	void event_emitter::run()
	{
        if(!this->_function.isset())return;

        _has_event=false;
        this->_function.invoke(this->_event_source);
	}

	event_emitter_waitor::event_emitter_waitor():_event_emitter(0)
	{

	}

	event_emitter_waitor::~event_emitter_waitor()
	{
		this->fire_from_handle.reset(0);
	}

	void event_emitter_waitor::clear()
	{
	
		if(_event_emitter)_event_emitter->clear();
		_event_emitter=0;

	    this->fire_from_handle.reset(0);
	}

	void event_emitter_waitor::on_callback(ref_class_i* fff)
	{
		mutex_lock_scope l(&this->fire_from_handle);

		this->fire_from_handle.reset(fff);
		event_signal::fire_event();
	}

	void event_emitter_waitor::on_destroy()
	{

	}

};