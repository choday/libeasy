#pragma once
#include "../ebase/ref_class.hpp"
#include "../ebase/executor.hpp"
#include "../ebase/event_emitter.hpp"
#include "../ebase/string.hpp"
#include "../ebase/buffer.hpp"
#include "socket_address.hpp"
namespace eio
{
    class socket_io:public ebase::ref_class<>
    {
    public:
        socket_io();
        ~socket_io();

		ebase::event_emitter				on_opened;
		ebase::event_emitter				on_error;
		ebase::event_emitter				on_closed;

		ebase::event_emitter				on_readable;
		ebase::event_emitter				on_writeable;

        virtual void            set_event_executor( ebase::executor* event_executor );
        virtual void            attach_socket_io(socket_io* next);
        socket_io*              get_next_socket_io();

		virtual bool			open( const ebase::string& host,const ebase::string& port_or_service );
		virtual bool			open(const socket_address& address );
        virtual bool            is_opened();
        virtual void            close();
  
        virtual bool            send( const ebase::buffer& data );
        virtual bool            recv( ebase::buffer& data );
        virtual bool            sendto( const ebase::buffer& data,const socket_address& to_address );
        virtual bool            recvfrom( ebase::buffer& data,socket_address* from_address=0 );

        virtual int             get_error_code() const;
        virtual const char*     get_error_message() const;
    protected:
        

		virtual void			notify_error(ref_class_i* fire_from_handle);
		virtual void			notify_opened(ref_class_i* fire_from_handle);
		virtual void			notify_closed(ref_class_i* fire_from_handle);

		virtual void			notify_readable(ref_class_i* fire_from_handle);
		virtual void			notify_writeable(ref_class_i* fire_from_handle);

        ebase::ref_ptr<socket_io>   _next_socket_io;
        ebase::executor*            _event_executor;
    };

    typedef ebase::ref_ptr<socket_io>   socket_io_ptr;
};