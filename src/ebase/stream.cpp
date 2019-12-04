#include "stream.hpp"

namespace ebase
{
	stream_writeable::stream_writeable():_is_end(false),_write_cache_size(4096)
	{
		on_drain.set_event_source( this );
		on_error.set_event_source( this );
		private_on_want_write.set_event_source( this );
	}

	void stream_writeable::set_event_executor(executor* event_executor)
	{
		on_drain.set_event_executor( event_executor );
		on_error.set_event_executor( event_executor );
	}

	bool stream_writeable::write(const buffer& buffer,bool _flush)
	{
        if(_is_end)return false;
		_buffer_list.lock();
		_buffer_list.push_back(buffer);
		_buffer_list.unlock();

		if(!_flush)return true;
		return this->flush();
	}

	bool stream_writeable::flush()
	{
		return this->private_on_want_write.fire();
	}

    ebase::buffer stream_writeable::private_pop_buffer()
    {
        ebase::buffer buffer;
		_buffer_list.lock();
		buffer = _buffer_list.pop_front();
		_buffer_list.unlock();

        if(buffer.size()==0)this->on_drain.fire();

        return buffer;
    }

    void stream_writeable::private_return_buffer(ebase::buffer& buffer)
    {
		_buffer_list.lock();
		_buffer_list.push_front(buffer);
		_buffer_list.unlock();
    }

	bool stream_writeable::end()
	{
		_is_end=true;

		return flush();
	}

	bool stream_writeable::is_end()
	{
		return _is_end;
	}

	int stream_writeable::get_write_cache_size()
	{
		return _buffer_list.data_size();
	}


    void stream_writeable::private_set_event_executor(executor* event_executor)
	{
		this->private_on_want_write.set_event_executor( event_executor );
	}

	stream_readable::stream_readable():_is_paused(false),_read_cache_size(4096)
	{
		on_data.set_event_source( this );
		on_error.set_event_source( this );
		on_end.set_event_source( this );

		private_on_want_read.set_event_source( this );
	}

	void stream_readable::set_event_executor(executor* event_executor)
	{
		on_data.set_event_executor( event_executor );
		on_error.set_event_executor( event_executor );
		on_end.set_event_executor( event_executor );
	}

	buffer stream_readable::read()
	{
		buffer bbb;
        
		_buffer_list.lock();
		bbb = _buffer_list.pop_front();
		_buffer_list.unlock();

        if(bbb.size()==0)private_on_want_read.fire();
		return bbb;
	}

    void stream_readable::private_push_buffer(const buffer& data)
    {
		_buffer_list.lock();
		_buffer_list.push_back(data);
		_buffer_list.unlock();

        on_data.fire();
    }

	bool stream_readable::pause()
	{
		_buffer_list.lock();
		_is_paused = true;
		_buffer_list.unlock();

        private_on_want_read.fire();
		return true;
	}

	bool stream_readable::resume()
	{
		_buffer_list.lock();
		_is_paused = false;
		_buffer_list.unlock();

        private_on_want_read.fire();
		return true;
	}

	bool stream_readable::is_paused()
	{
		bool result = false;
		_buffer_list.lock();
		result = _is_paused;
        if(_buffer_list.data_size()>=_read_cache_size)result=false;
		_buffer_list.unlock();

		return result;
	}

	bool stream_readable::pip(stream_writeable* writeable)
	{

		return true;
	}


    int stream_readable::get_read_cache_size()
    {
        return _buffer_list.data_size();
    }

    void stream_readable::private_set_event_executor(executor* event_executor)
	{
		private_on_want_read.set_event_executor(event_executor);
	}

    stream::stream()
	{

		
	}

	void stream::set_event_executor(executor* event_executor)
	{
        on_opened.set_event_executor(event_executor);
        stream_readable::set_event_executor(event_executor);
        stream_writeable::set_event_executor(event_executor);
	}

    void stream::private_set_event_executor(executor* event_executor)
    {
        stream_readable::private_set_event_executor(event_executor);
        stream_writeable::private_set_event_executor(event_executor);
    }

    void stream::private_notify_error(int code)
    {
        this->error.set_system_error(code);
        stream_readable::error.set_system_error(code);
        stream_writeable::error.set_system_error(code);

        this->on_error.fire();
        stream_readable::on_error.fire();
        stream_writeable::on_error.fire();
    }

    void stream::private_notify_error(const ebase::error& e)
    {
        this->error = e;
        stream_readable::error=e;
        stream_writeable::error=e;

        this->on_error.fire();
        stream_readable::on_error.fire();
        stream_writeable::on_error.fire();
    }

};