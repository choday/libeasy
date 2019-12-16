#pragma once
#include "../eio/socket_io.hpp"
#include "../eio/socket_native.hpp"
#include "../ebase/string.hpp"
#include "../ebase/executor.hpp"
#include "../ebase/ref_tree.hpp"
#include "../ebase/ref_list.hpp"
#include "../eio/ssl/ssl_context.hpp"
#include "http_socket.hpp"
namespace ehttp
{

    class http_virtual_server:public ebase::executor_single_thread,public ebase::ref_tree::entry
    {
    public:
        http_virtual_server(ebase::executor* event_executor=0);
        ~http_virtual_server();

        ebase::string           directory;
        ebase::string           allowed_methods;//405 METHOD_NOT_ALLOWED
        ebase::event_emitter    on_request;


        eio::ssl_context_ptr    startup_ssl(eio::ssl_context* exist_context=0);
        void                    set_event_executor( ebase::executor* event_executor );
    protected:
        friend class http_server;
        ebase::string           _host;
        eio::ssl_context_ptr    _ssl_context;
        ebase::ref_list         _request_list;

        bool                    add_request(http_socket* s);
    private:
        virtual int		    compare_rbtree_entry( ebase::ref_tree::entry* right_value ) override;
        virtual int		    compare_rbtree_find_value(void* pfind_value ) override;
    };

}