#pragma once
#include "../ebase/stream.hpp"
#include "socket_native.hpp"
#include "ssl/ssl_filter.hpp"
#include "stream_socket_filter.hpp"

namespace eio
{
    //ע���麯���ĵ��ã�����󶨵���һ���麯������ô����������ʵ��
    class stream_socket:public ebase::stream,public stream_socket_notify_i
	{
	public:
        stream_socket();
        ~stream_socket();

        void                    assign(socket_ptr ptr);

		bool					open( const ebase::string& host,const ebase::string& port_or_service );//���ӳɹ���on_drain������
		bool					open(const socket_address& address );
        
        void					close();//�����ebase::stream::end(),���������ٹر�socket

        bool                    ssl_startup(ssl_context* context=0);

        bool                    ssl_connect(ssl_filter* ssl=0);
        bool                    ssl_accept(ssl_filter* ssl=0);

        void                    add_filter(stream_socket_filter_i* filter);
    private:
        socket_ptr              _socket;
        ssl_context_ptr         _ssl_context;
        ssl_filter_ptr          _ssl_filter;

        ebase::ref_ptr<stream_socket_filter_i>  _outter_filter;
        stream_socket_filter_i*                 _socket_filter;
        socket_filter_wrap                      _socket_filter_wrap;

        virtual void                    filter_notify_opened() override;
        virtual void					filter_notify_error(const ebase::error& e) override;
		virtual void					filter_notify_closed() override;

		virtual void					filter_notify_readable() override;
		virtual void					filter_notify_writeable() override;

        void                            on_stream_want_read(ref_class_i* fire_from_handle);
        void                            on_stream_want_write(ref_class_i* fire_from_handle);

        bool                            do_flush();//����������ȫ��ѹ����һ�㣬����true,���򷵻�false
        bool                            do_fetch();//�²�������ȫ�����꣬����true
	};

    typedef ebase::ref_ptr<stream_socket>   stream_socket_ptr;
};