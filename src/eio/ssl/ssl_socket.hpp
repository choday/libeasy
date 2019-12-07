#pragma once
#include "../socket_io.hpp"
#include "../../ebase/error.hpp"
#include "openssl_context.hpp"

namespace eio
{

    class ssl_socket:public socket_io
    {
    public:
        ssl_socket(openssl_context* ctx);
        ~ssl_socket();

        void                            ssl_set_connect();    
        void                            ssl_set_accept();
        bool                            ssl_do_handshake();
        bool                            ssl_set_host_name(const char* name);

		virtual bool			        open( const ebase::string& host,const ebase::string& port_or_service ) override;
		virtual bool			        open(const socket_address& address ) override;
        virtual bool                    is_opened() override;
        virtual bool                    send( const ebase::buffer& data ) override;
        virtual bool                    recv( ebase::buffer& data ) override;
        virtual int                     get_error_code() const override;
        virtual const char*             get_error_message() const override;
    private:
        ebase::ref_ptr<ssl_context>     _context;
        SSL *                           _ssl;
        BIO *                           _wbio;
        BIO*                            _rbio;
        ebase::buffer                   _cache_write_buffer;
        ebase::buffer                   _cache_read_buffer;
        ebase::error                    _error;

        bool                            do_error(int n);
        bool                            do_flush();//本层数据已全部压入下一层，返回true,否则返回false
        bool                            do_fetch();//下层数据已全部读完，返回true

		virtual void			notify_opened(ref_class_i* fire_from_handle) override;

		virtual void			notify_readable(ref_class_i* fire_from_handle) override;
		virtual void			notify_writeable(ref_class_i* fire_from_handle) override;
    };
};