#pragma once
#include "../socket_io.hpp"
#include "../../ebase/error.hpp"
#include "openssl_context.hpp"

namespace eio
{

    class ssl_socket:public socket_io_wrap
    {
    public:
        ssl_socket(openssl_context* ctx=0);
        ~ssl_socket();

        void                            ssl_set_connect();    
        void                            ssl_set_accept();
        bool                            ssl_set_host_name(const char* name);

		virtual bool			        open( const ebase::string& host,const ebase::string& port_or_service ) override;
		virtual bool			        open(const socket_address& address ) override;
        virtual bool                    is_opened() override;
        virtual int                     write( const void* data,int len ) override;
        virtual int                     read( void* data,int len ) override;
        virtual int                     write_buffer( const ebase::buffer& data ) override;
        virtual int                     read_buffer( ebase::buffer& data ) override;

        virtual int                     get_nread_size() const override;
        virtual int                     get_error_code() const override;
        virtual const char*             get_error_message() const override;
    private:
        ebase::ref_ptr<ssl_context>     _context;
        SSL *                           _ssl;
        BIO *                           _wbio;
        BIO*                            _rbio;
        ebase::buffer                   _out_cache;
        ebase::buffer                   _in_cache;
        ebase::error                    _error;
        bool                            _ssl_connected;
        int                             _max_read_cache_size;

        int                             update_ssl_in();//返回pendding数量

        bool                            do_flush_loop();//本层数据已全部压入下一层，返回true,否则返回false
        bool                            do_fetch_once();//有数据读入到ssl,返回true
        bool                            do_handshake_loop();//需要再次调用返回true
        bool                            check_error(int n);//出错返回true

		virtual void			        notify_opened(ref_class_i* fire_from_handle) override;
		virtual void			        notify_readable(ref_class_i* fire_from_handle) override;
		virtual void			        notify_writeable(ref_class_i* fire_from_handle) override;
    };
};