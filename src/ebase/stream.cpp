#include "stream.hpp"

namespace ebase
{
	stream_writeable::stream_writeable():_is_end(false),_write_cache_size(4096)
	{
		on_drain.set_event_source( this );
		on_error.set_event_source( this );
		private_on_want_pop_buffer.set_event_source( this );
	}

	void stream_writeable::set_event_executor(executor* event_executor)
	{
		on_drain.set_event_executor( event_executor );
		on_error.set_event_executor( event_executor );
	}

	bool stream_writeable::write(const buffer& buffer,bool flush_now)
	{
        if(_is_end)return false;

		_buffer_list.lock();
		_buffer_list.push_back(buffer);
		_buffer_list.unlock();

        if(flush_now)this->private_on_want_pop_buffer.fire();

		return true;
	}

	bool stream_writeable::flush()
	{
		this->private_on_want_pop_buffer.fire();
        return true;
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

		this->private_on_want_pop_buffer.fire();
        return true;
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
		this->private_on_want_pop_buffer.set_event_executor( event_executor );
	}

	stream_readable::stream_readable():_is_paused(false),_read_cache_size(4096),_is_eof(false)
	{
		on_data.set_event_source( this );
		on_end.set_event_source( this );

		private_on_want_push_buffer.set_event_source( this );
	}

	void stream_readable::set_event_executor(executor* event_executor)
	{
		on_data.set_event_executor( event_executor );
		on_end.set_event_executor( event_executor );
	}

    bool stream_readable::read(buffer& data)
	{
        if( !_is_eof && _buffer_list.data_size() == 0 )private_on_want_push_buffer.fire();

		_buffer_list.lock();
		data = _buffer_list.pop_front();
		_buffer_list.unlock();


		return data.size()>0;
	}

    void stream_readable::private_push_buffer(const buffer& data)
    {
		_buffer_list.lock();
		_buffer_list.push_back(data);
		_buffer_list.unlock();

        on_data.fire();
    }

    void stream_readable::private_notify_end()
    {
        _is_eof=true;
        on_end.fire();
    }

    bool stream_readable::pause()
	{
		_buffer_list.lock();
		_is_paused = true;
		_buffer_list.unlock();

		return true;
	}

	bool stream_readable::resume()
	{
        bool result = false;
		_buffer_list.lock();
		_is_paused = false;
        if(_buffer_list.data_size()>=_read_cache_size)result=true;
		_buffer_list.unlock();

        if(result)return false;
        private_on_want_push_buffer.fire();
		return true;
	}

	bool stream_readable::is_paused()
	{
        if(_is_eof)return true;

		bool result = false;
		_buffer_list.lock();
		result = _is_paused;
        if(_buffer_list.data_size()>=_read_cache_size)result=true;
		_buffer_list.unlock();

		return result;
	}

	bool stream_readable::pip(stream_writeable* writeable)
	{

		return true;
	}


    int stream_readable::get_nread_size()
    {
        return _buffer_list.data_size();
    }

    void stream_readable::private_set_event_executor(executor* event_executor)
	{
		private_on_want_push_buffer.set_event_executor(event_executor);
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

    void stream::private_notify_error(int code,const char* msg)
    {
        assert(this->error.code);
        if(this->error.code)return;

        if(msg)
        {
            this->error.set_user_error(code,msg );
        }else
        {
            this->error.set_system_error(code);
        }
        
        stream_readable::error=this->error;
        stream_writeable::error=this->error;

        stream_writeable::on_error.fire();
    }

    void stream::private_notify_error(const ebase::error& e)
    {
        assert(this->error.code);
        if(this->error.code)return;

        this->error = e;
        stream_readable::error=this->error;
        stream_writeable::error=this->error;

        stream_writeable::on_error.fire();
    }

};