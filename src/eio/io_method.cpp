#include "io_method.hpp"

namespace eio
{

    io_method::io_method()
    {
		on_readable.set_event_source(this);
		on_writeable.set_event_source(this);
		on_closed.set_event_source(this);
		on_error.set_event_source(this);
    }

    void io_method::set_event_executor(ebase::executor* event_executor)
    {
		on_readable.set_event_executor(event_executor);
		on_writeable.set_event_executor(event_executor);
		on_closed.set_event_executor(event_executor);
		on_error.set_event_executor(event_executor);
    }

    int io_method::read_buffer(ebase::buffer& data)
    {
        int capacity = data.capacity();
        if(!capacity)capacity = 1024-ebase::buffer::header_size;
		void* p = data.alloc(capacity);

        int result = this->read( p,capacity );
        if( result>0 )
        {
            data.resize(result,true);
        }else
        {
            data.resize(0);
        }

        return result;        
    }

    int io_method::write_buffer(const ebase::buffer& data)
    {
        return this->write( data.data(),data.size() );
    }


}
