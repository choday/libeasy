#include "http_connection.hpp"
#include "../eio/ssl/ssl_socket.hpp"
#include "../estream/zlib_stream.hpp"

#include "stdio.h"
namespace ehttp
{

    http_connection::http_connection()
    {
        _is_server=true;
        _current_http_parser=&request;
        _current_http_parser->set_callback(this);
        _is_http_opened=false;
        _max_http_read_cache=1024-ebase::buffer::header_size;
        _http_fetch_once_body_cache = 0;
    }

    http_connection::~http_connection()
    {

    }
    bool http_connection::open(const ebase::string& host,const ebase::string& port_or_service)
    {
        _is_server=false;
        _current_http_parser=&response;
        _current_http_parser->set_callback(this);

        if(request.url.schema=="https")
        {
            eio::ssl_socket* s = new eio::ssl_socket();
            s->ssl_set_host_name(request.url.host.c_str());
            socket_io_filter::attach_socket_io(s);
        }

        if(host.length())
        {
            return socket_io_filter::open(host,port_or_service);
        }else if( request.url.host.length() )
        {
            return socket_io_filter::open(request.url.host,request.url.port.length()?request.url.port:request.url.schema);
        }

        return false;
    }

    
    bool http_connection::open_request(const ebase::string& method,const ebase::string& url)
    {
        if(!request.url.parse( url,false ) )return false;

        request.method = method;

        return open();
    }

    int http_connection::write_buffer(const ebase::buffer& data)
    {
        if(!_is_http_opened)return -1;

        if( do_flush() < 0 )return -1;

        if( _cache_out.size() )return 0;
        _cache_out=data;
        do_flush();

        return data.size();
    }

    int http_connection::read_buffer(ebase::buffer& data)
    {
        if(_http_cache_data.size())
        {
            data=_http_cache_data;
            _http_cache_data.resize(0);
            return true;
        }

        int capacity = data.capacity();
        if(!capacity)capacity = 1024-ebase::buffer::header_size;

        data.alloc(capacity);

        do{
            if(!do_fetch_once(data))break;
        }while(data.size()==0);

        return data.size();
    }

    void http_connection::notify_error(ref_class_i* fire_from_handle)
    {
        eio::socket_io_filter::notify_error(fire_from_handle);
    }

    void http_connection::notify_opened(ref_class_i* fire_from_handle)
    {
        ebase::buffer data = request.make_buffer();

        //ebase::string temp( (const char*)data.data(),data.size() );
        //printf("send\n%s\n",temp.c_str() );

        int result = eio::socket_io_filter::_next_socket_io->write_buffer(data);
        if(result>0)data.resize(0);
        else if(_cache_out.size()==0)
        {
            _cache_out = data;
        }else
        {
            assert(result);
        }
    }

    void http_connection::notify_closed(ref_class_i* fire_from_handle)
    {
        eio::socket_io_filter::notify_closed(fire_from_handle);
    }

    void http_connection::notify_readable(ref_class_i* fire_from_handle)
    {
        while( _http_cache_data.size()<_max_http_read_cache )
        {
            if(!do_fetch_once(_http_cache_data))break;
        }

        if( _http_cache_data.size() )
        {
            eio::socket_io_filter::notify_readable(this);
        }
    }

    void http_connection::notify_writeable(ref_class_i* fire_from_handle)
    {
        //eio::socket_io_filter::notify_writeable(fire_from_handle);
    }

    bool http_connection::do_fetch_once(ebase::buffer& out_buffer)
    {
        if(!_next_socket_io.valid())return false;
        
        if(out_buffer.capacity()==0)out_buffer.alloc(1024-ebase::buffer::header_size);

        if(out_buffer.capacity()==out_buffer.size())return false;
        _http_fetch_once_body_cache=&out_buffer;

        if(_current_http_parser->is_end())return false;

        if(!_cache_in.size())
        {
            if(_next_socket_io->read_buffer(_cache_in)<=0)return false;
        }

        assert(_cache_in.size());

        size_t result = _current_http_parser->parse( (const char *)_cache_in.data(),_cache_in.size() );

        if(result==_cache_in.size() )
        {
            _cache_in.resize(0);
        }else if(result>0)
        {//等待下一次解析
            _cache_in.cut_data((int)result);
        }else
        {//等待下一次解析
            return false;
        }
        return true;
    }

    int http_connection::do_flush()
    {
        int n=0;
        if(_cache_out.size())
        {
            n=socket_io_filter::_next_socket_io->write_buffer( _cache_out );
            if( n > 0 )
            {
                _cache_out.resize(0);
                return n;
            }
        }
        return n;
    }

    bool http_connection::on_http_protocol_headers_complete()
    {
        _is_http_opened=true;

/*
        estream::zlib_stream* compress_stream = 0;

        if(_is_server && request.accept_encoding.length() )
        {

            request.accept_encoding.make_lower();
            if(request.accept_encoding.find("gzip")>=0)
            {
                compress_stream=new estream::zlib_stream();
                compress_stream->init( true,estream::zlib_stream::deflate_format_gzip );
            }else if(request.accept_encoding.find("deflate")>=0)
            {
                compress_stream=new estream::zlib_stream();
                compress_stream->init( true,estream::zlib_stream::deflate_format_raw );
            }else
            {
                //unsupport encoding
            }


        }

        if(compress_stream)
        {
            _stream_writeable=compress_stream;
        }else
        {
            _stream_writeable = new ebase::stream_writeable();
        }
*/

        estream::zlib_stream* uncompress_stream = 0;

        if(_current_http_parser->content_encoding.length())
        {
            if( _current_http_parser->content_encoding.compare_ignore_case("gzip")==0 )
            {
                uncompress_stream=new estream::zlib_stream();
                uncompress_stream->init( false,estream::zlib_stream::deflate_format_gzip );

            }else if( _current_http_parser->content_encoding.compare_ignore_case("deflate")==0 )
            {
                uncompress_stream=new estream::zlib_stream();
                uncompress_stream->init( false,estream::zlib_stream::deflate_format_raw );
            }else
            {
                //error unsupport encoding

                return false;
            }
      
        }


        eio::socket_io_filter::notify_opened(this);
        return true;
    }

    void http_connection::on_http_protocol_body(const char* data,size_t len)
    {
        if(_cache_in.data()==data && _cache_in.size() == len )
        {
            _http_fetch_once_body_cache->append( _cache_in );
        }else
        {
            _http_fetch_once_body_cache->append( data,len );
        }
    }

    void http_connection::on_http_protocol_complete()
    {

    }

};