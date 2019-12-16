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

    bool stream_readable::private_push_buffer(const buffer& data)
    {
        if(_is_eof)return false;
        bool result = false;
		_buffer_list.lock();
        _buffer_list.push_back(data);
		_buffer_list.unlock();

        if(_stream_writeable.valid())
        {
            flush_to_pip_stream();
        }else
        {
            on_data.fire();
        }
        return true;
    }

    void stream_readable::private_notify_end()
    {
        _is_eof=true;

        if(_stream_writeable.valid())
            flush_to_pip_stream();
        else
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
        if(writeable->is_end())return false;

        _stream_writeable = writeable;
        _stream_writeable->on_drain.set_function( &stream_readable::on_pip_drain_event,this );
        _stream_writeable->on_error.set_function( &stream_readable::on_pip_error_event,this );

        flush_to_pip_stream();

		return true;
	}

    void stream_readable::flush_to_pip_stream()
    {
        if(!_stream_writeable.valid())return;

		_buffer_list.lock();
        ebase::buffer data = _buffer_list.pop_front();
        while(data.size())
        {
            if( !_stream_writeable->write(data,true) )
            {
                _buffer_list.push_front(data);
                break;
            }

            if(_stream_writeable->get_write_cache_size()>_read_cache_size)break;

            data = _buffer_list.pop_front();
        }

        if(_stream_writeable->is_end())
        {
            _buffer_list.clear();
        }
		_buffer_list.unlock();
        
        if(_buffer_list.data_size()==0&&_is_eof)
        {
            _stream_writeable->end();
            _stream_writeable.reset();
        }

        if( !_is_eof && _buffer_list.data_size() == 0 )private_on_want_push_buffer.fire();
    }

    void stream_readable::on_pip_error_event(ebase::ref_class_i* from)
    {
        if(_stream_writeable.valid())this->error=_stream_writeable->error;
        _stream_writeable.reset();

        _is_eof=true;
        _buffer_list.lock();
        _buffer_list.clear();
        _buffer_list.lock();
    }

    void stream_readable::on_pip_drain_event(ebase::ref_class_i* from)
    {
        flush_to_pip_stream();
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