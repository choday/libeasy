#include "stream_socket.hpp"
#include "ssl/openssl_filter.hpp"

namespace eio
{
    stream_socket::stream_socket()
    {
        _socket_filter_wrap.set_out_ref(this);
        _socket_filter_wrap.filter_set_notify(this);
        _socket_filter = &_socket_filter_wrap;

        stream::private_on_want_write.set_function( &stream_socket::on_stream_want_write,this );
        stream::private_on_want_read.set_function( &stream_socket::on_stream_want_read,this );
    }

    stream_socket::~stream_socket()
    {

    }

    bool stream_socket::open(const ebase::string& host,const ebase::string& port_or_service)
    {

        this->assign(socket::create_instance());

        this->ssl_connect();

        return this->_socket_filter->filter_open( host,port_or_service );
    }

    bool stream_socket::open(const socket_address& address)
    {

        this->assign(socket::create_instance());

        this->ssl_connect();

        
        return this->_socket_filter->filter_open( address );
    }

    void stream_socket::assign(socket_ptr ptr)
    {
        _socket=ptr;

        stream::private_set_event_executor(_socket);

        _socket_filter_wrap.assign(_socket,_socket);
    }

    void stream_socket::close()
    {
        _socket_filter_wrap.filter_close();
        _ssl_context=0;
        _ssl_filter = 0;
        _outter_filter = 0;
        _socket = 0;
    }

    bool stream_socket::ssl_startup(ssl_context* context/*=0*/)
    {
        if(context)
            _ssl_context=context;
        else
            _ssl_context = new openssl_context();
        return true;
    }

    bool stream_socket::ssl_connect(ssl_filter* ssl)
    {
        if(_ssl_filter.valid()&&!ssl)return false;

        if(_ssl_filter.valid())
        {
            //????
        }

        if(ssl)_ssl_filter=ssl;
        if(!_ssl_filter.valid() && _ssl_context.valid() )_ssl_filter = _ssl_context->create_ssl_filter();
        
        this->add_filter(_ssl_filter);

        _ssl_filter->ssl_set_connect();

        return true;
    }

    bool stream_socket::ssl_accept(ssl_filter* ssl)
    {
        if(_ssl_filter.valid()&&!ssl)return false;

        if(_ssl_filter.valid())
        {
            //????
        }

        if(ssl)_ssl_filter=ssl;
        if(!_ssl_filter.valid() && _ssl_context.valid() )_ssl_filter = _ssl_context->create_ssl_filter();
        
        this->add_filter(_ssl_filter);

        _ssl_filter->ssl_set_accept();

        return true;
    }

    void stream_socket::add_filter(stream_socket_filter_i* filter)
    {

        filter->filter_set_next(_socket_filter);
        filter->filter_set_notify(this);

        _socket_filter=filter;

        _outter_filter = filter;
    }

    void stream_socket::filter_notify_opened()
    {
        stream::on_opened.fire();
        stream::on_drain.fire();
    }

    void stream_socket::filter_notify_error(const ebase::error& e)
    {
        stream::private_notify_error(e);
    }

    void stream_socket::filter_notify_closed()
    {
        stream::on_end.fire();
    }

    void stream_socket::filter_notify_readable()
    {
        do_fetch();
        
    }

    void stream_socket::filter_notify_writeable()
    {
        do_flush();
        
    }

    void stream_socket::on_stream_want_read(ref_class_i* fire_from_handle)
    {
        do_fetch();
    }

    void stream_socket::on_stream_want_write(ref_class_i* fire_from_handle)
    {
        do_flush();
    }

    bool stream_socket::do_flush()
    {
        ebase::buffer data = stream::private_pop_buffer();

        while(data.size())
        {
            if(!this->_socket_filter->filter_write(data))
            {
                stream::private_return_buffer(data);
                return false;
            }
        }
        on_drain.fire();
        return true;
    }

    bool stream_socket::do_fetch()
    {
        ebase::buffer data;
        while( !stream::is_paused() )
        {
            if(!this->_socket_filter->filter_read(data))return false;
            stream::private_push_buffer( data );
        }
        
        on_data.fire();
        return true;
    }

};