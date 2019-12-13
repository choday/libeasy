#include "socket_io.hpp"
#include "socket_native.hpp"
namespace eio
{
    void socket_io::set_event_executor(ebase::executor* event_executor)
    {
		on_opened.set_event_source_and_executore(this,event_executor);
		on_readable.set_event_source_and_executore(this,event_executor);
		on_writeable.set_event_source_and_executore(this,event_executor);
		on_closed.set_event_source_and_executore(this,event_executor);
		on_error.set_event_source_and_executore(this,event_executor);
    }
    void socket_io::clear_all_event()
    {

    }


    socket_io_wrap::socket_io_wrap()
    {

    }

    socket_io_wrap::~socket_io_wrap()
    {

    }

    void socket_io_wrap::attach_socket_io(socket_io* next)
    {
        _next_socket_io=next;

        next->on_opened.set_function(&socket_io_wrap::notify_opened,this);
        next->on_error.set_function(&socket_io_wrap::notify_error,this);
        next->on_closed.set_function(&socket_io_wrap::notify_closed,this);
        next->on_readable.set_function(&socket_io_wrap::notify_readable,this);
        next->on_writeable.set_function(&socket_io_wrap::notify_writeable,this);
    }

    void socket_io_wrap::set_event_executor(ebase::executor* event_executor)
    {
        if(_next_socket_io.valid())_next_socket_io->set_event_executor(event_executor);

        socket_io::set_event_executor( event_executor);
    }

    eio::socket_io* socket_io_wrap::get_next_socket_io()
    {
        return _next_socket_io.get();
    }

    bool socket_io_wrap::open(const ebase::string& host,const ebase::string& port_or_service)
    {
        if(!_next_socket_io.valid())attach_socket_io(socket_native::create_instance());
        return _next_socket_io->open(host,port_or_service);
    }

    bool socket_io_wrap::open(const socket_address& address)
    {
        if(!_next_socket_io.valid())attach_socket_io(socket_native::create_instance());
        return _next_socket_io->open(address);
    }

    bool socket_io_wrap::is_opened()
    {
        if(!_next_socket_io.valid())return false;
        return _next_socket_io->is_opened();
    }

    void socket_io_wrap::close(bool delay)
    {
        if(!_next_socket_io.valid())return;
        _next_socket_io->close(delay);
        socket_io::clear_all_event();
    }

    int socket_io_wrap::read(void* data,int len)
    {
        if(!_next_socket_io.valid())return -1;
        return _next_socket_io->read(data,len);
    }

    int socket_io_wrap::write(const void* data,int len)
    {
        if(!_next_socket_io.valid())return -1;
        return _next_socket_io->write(data,len);
    }

    int socket_io_wrap::read_buffer(ebase::buffer& data)
    {
        if(!_next_socket_io.valid())return false;
        return _next_socket_io->read_buffer(data);
    }

    int socket_io_wrap::write_buffer(const ebase::buffer& data)
    {
        if(!_next_socket_io.valid())return false;
        return _next_socket_io->write_buffer(data);
    }

    int socket_io_wrap::get_nread_size() const 
    {
        if(!_next_socket_io.valid())return 0;
        return _next_socket_io->get_nread_size();
    }

    int socket_io_wrap::get_error_code() const
    {
        if(!_next_socket_io.valid())return -1;
        return _next_socket_io->get_error_code();
    }

    const char* socket_io_wrap::get_error_message() const
    {
        if(!_next_socket_io.valid())return "_next_socket_io is invalid";
        return _next_socket_io->get_error_message();
    }

    void socket_io_wrap::notify_error(ref_class_i* fire_from_handle)
    {
        on_error.fire();
    }

    void socket_io_wrap::notify_opened(ref_class_i* fire_from_handle)
    {
        on_opened.fire();
    }

    void socket_io_wrap::notify_closed(ref_class_i* fire_from_handle)
    {
        on_closed.fire();
    }

    void socket_io_wrap::notify_readable(ref_class_i* fire_from_handle)
    {
        on_readable.fire();
    }

    void socket_io_wrap::notify_writeable(ref_class_i* fire_from_handle)
    {
        on_writeable.fire();
    }

};