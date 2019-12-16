#include "http_virtual_server.hpp"
namespace ehttp
{

    http_virtual_server::http_virtual_server(ebase::executor* event_executor)
    {
        ebase::ref_tree::entry::set_holder(this);
        on_request.set_event_source_and_executore(this,event_executor);
        allowed_methods="GET,POST,OPTIONS";
    }

    http_virtual_server::~http_virtual_server()
    {

    }

    eio::ssl_context_ptr http_virtual_server::startup_ssl(eio::ssl_context* exist_context/*=0*/)
    {
        _ssl_context=exist_context;
        return _ssl_context.get();
    }

    void http_virtual_server::set_event_executor(ebase::executor* event_executor)
    {
        on_request.set_event_executor(event_executor);
    }

    bool http_virtual_server::add_request(http_socket* s)
    {
        return _request_list.push_back(s);
    }

    int http_virtual_server::compare_rbtree_entry(ebase::ref_tree::entry* right_value)
    {
        http_virtual_server* right = right_value->get_holder<http_virtual_server>();
        return _host.compare(right->_host);
    }

    int http_virtual_server::compare_rbtree_find_value(void* pfind_value)
    {
        ebase::string* pfindhost=(ebase::string*)pfind_value;
        return _host.compare(*pfindhost);
    }

}