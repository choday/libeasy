#include "socket_io.hpp"
#include "socket_native.hpp"
namespace eio
{

    socket_io::socket_io():_event_executor(0)
    {

    }

    socket_io::~socket_io()
    {

    }

    void socket_io::attach_socket_io(socket_io* next)
    {
        _next_socket_io=next;

        next->on_opened.set_function(&socket_io::notify_opened,this);
        next->on_error.set_function(&socket_io::notify_error,this);
        next->on_closed.set_function(&socket_io::notify_closed,this);
        next->on_readable.set_function(&socket_io::notify_readable,this);
        next->on_writeable.set_function(&socket_io::notify_writeable,this);
    }

    void socket_io::set_event_executor(ebase::executor* event_executor)
    {
        _event_executor=  event_executor;
        
        if(_next_socket_io.valid())_next_socket_io->set_event_executor(event_executor);

		on_opened.set_event_source_and_executore(this,event_executor);
		on_readable.set_event_source_and_executore(this,event_executor);
		on_writeable.set_event_source_and_executore(this,event_executor);
		on_closed.set_event_source_and_executore(this,event_executor);
		on_error.set_event_source_and_executore(this,event_executor);

    }

    eio::socket_io* socket_io::get_next_socket_io()
    {
        return _next_socket_io.get();
    }

    bool socket_io::open(const ebase::string& host,const ebase::string& port_or_service)
    {
        if(!_next_socket_io.valid())attach_socket_io(socket_native::create_instance());
        return _next_socket_io->open(host,port_or_service);
    }

    bool socket_io::open(const socket_address& address)
    {
        if(!_next_socket_io.valid())attach_socket_io(socket_native::create_instance());
        return _next_socket_io->open(address);
    }

    bool socket_io::is_opened()
    {
        if(!_next_socket_io.valid())return false;
        return _next_socket_io->is_opened();
    }

    void socket_io::close()
    {
        if(!_next_socket_io.valid())return;
        _next_socket_io->close();
		on_opened.clear();
		on_readable.clear();
		on_writeable.clear();
		on_closed.clear();
		on_error.clear();
    }

    bool socket_io::send(const ebase::buffer& data)
    {
        if(!_next_socket_io.valid())return false;
        return _next_socket_io->send(data);
    }

    bool socket_io::recv(ebase::buffer& data)
    {
        if(!_next_socket_io.valid())return false;
        return _next_socket_io->recv(data);
    }

    bool socket_io::sendto(const ebase::buffer& data,const socket_address& to_address)
    {
        if(!_next_socket_io.valid())attach_socket_io(socket_native::create_instance());
        return _next_socket_io->sendto(data,to_address);
    }

    bool socket_io::recvfrom(ebase::buffer& data,socket_address* from_address/*=0 */)
    {
        if(!_next_socket_io.valid())return false;
        return _next_socket_io->recvfrom(data,from_address);
    }

    int socket_io::get_error_code() const
    {
        if(!_next_socket_io.valid())return false;
        return _next_socket_io->get_error_code();
    }

    const char* socket_io::get_error_message() const
    {
        if(!_next_socket_io.valid())return "_next_socket_io is invalid";
        return _next_socket_io->get_error_message();
    }

    void socket_io::notify_error(ref_class_i* fire_from_handle)
    {
        on_error.fire();
    }

    void socket_io::notify_opened(ref_class_i* fire_from_handle)
    {
        on_opened.fire();
    }

    void socket_io::notify_closed(ref_class_i* fire_from_handle)
    {
        on_closed.fire();
    }

    void socket_io::notify_readable(ref_class_i* fire_from_handle)
    {
        on_readable.fire();
    }

    void socket_io::notify_writeable(ref_class_i* fire_from_handle)
    {
        on_writeable.fire();
    }

};