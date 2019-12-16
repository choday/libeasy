#include "http_socket.hpp"
#include "../eio/ssl/ssl_socket.hpp"
#include "../estream/zlib_stream.hpp"

#include "stdio.h"
namespace ehttp
{
    http_socket::http_socket()
    {
        content_nread=0;
        _head_cache_in_offset=0;
        _is_http_opened=false;

        _current_http_parser=0;
        max_head_cache_size=1024-ebase::buffer::header_size;
        max_head_line_size=1024-ebase::buffer::header_size;
        _head_cache_in_line_end=0;
        _read_body_type = read_body_need_header;
    }

    http_socket::~http_socket()
    {

    }

    void http_socket::init(bool is_server/*=true*/)
    {
        _is_server=is_server;

        if(is_server)
        {
            _current_http_parser=&request;
        }else
        {
            _current_http_parser=&response;

            if(!_next_socket_io.valid()&&request.url.schema=="https")
            {
                eio::ssl_socket* s = new eio::ssl_socket();
                s->ssl_set_host_name(request.url.host.c_str());
                socket_io_filter::attach_socket_io(s);
            }
        }

        _current_http_parser->set_callback(this);

        if(is_server&&!_is_http_opened&&_next_socket_io.valid()&&_next_socket_io->is_opened())do_parse_header();
    }

    bool http_socket::open(const ebase::string& host,const ebase::string& port_or_service)
    {
        init(false);

        if(host.length())
        {
            return socket_io_filter::open(host,port_or_service);
        }else
        {
            return open();
        }
    }

    bool http_socket::open(const eio::socket_address& address)
    {
        init(false);

        if(address.size())
        {
            return socket_io_filter::open(address);
        }else
        {
            return open();
        }
    }

    bool http_socket::open()
    {
        init(false);

        if( request.url.host.length() )
        {
            return socket_io_filter::open(request.url.host,request.url.port.length()?request.url.port:request.url.schema);
        }else
        {
            return false;
        }
    }

    int http_socket::write_http_body(const void* data,int len)
    {
        if(!_is_http_opened)return -1;
        if(_head_cache_out.size()>0)return 0;

        //这里需要处理chunked
        return _next_socket_io->write(data,len);
    }

    int http_socket::read_http_body(void* data,int len)
    {
        int result = -1;
        int total_size = 0;

        char* p=(char*)data;

        total_size=this->do_read_body( p,len );
        if(total_size<=0)return total_size;

        p += total_size;
        len -= total_size;

        while(len>0) 
        {
            result=this->do_read_body( p,len );
            if(result<=0)break;
            if(result>0)
            {
                p += result;
                len -= result;
                total_size+=result;
            }
        }

        return total_size;
    }

    int http_socket::write(const void* data,int len)
    {
        return write_http_body(data,len);
    }

    int http_socket::read(void* data,int len)
    {
        return read_http_body(data,len);
    }

    void http_socket::notify_error(ref_class_i* fire_from_handle)
    {
        eio::socket_io_filter::notify_error(fire_from_handle);
    }

    int http_socket::send_response(int status_code,const ebase::string& status_msg)
    {
        if(0==status_code)status_code=response.status_code;

        response.status_code = status_code;

        if(status_msg.length())
            response.status_message = status_msg;
        else 
            response.status_message = http_status_str((http_status)status_code);

        _head_cache_out = response.make_buffer();

        int result = eio::socket_io_filter::_next_socket_io->write_buffer(_head_cache_out);
        if(result>0)_head_cache_out.clear();
        
        return result;
    }


    int http_socket::send_response(const ebase::string& data/*=""*/)
    {
        send_response(200);
        
        return 0;
    }

    void http_socket::notify_opened(ref_class_i* fire_from_handle)
    {
        if(!_is_server)
        {
            _head_cache_out = this->request.make_buffer();
            if( _next_socket_io->write_buffer(_head_cache_out ) <= 0 )return;
            _head_cache_out.clear();
        }
    }

    void http_socket::notify_closed(ref_class_i* fire_from_handle)
    {
        eio::socket_io_filter::notify_closed(fire_from_handle);
    }

    void http_socket::notify_readable(ref_class_i* fire_from_handle)
    {
        if(!_is_http_opened)
        {
            if(do_parse_header()<=0)return ;
        }

        if(read_body_need_thunked_header==_read_body_type)
        {
            if(do_fetch_chunk_header()>0)
            {
                eio::socket_io_filter::notify_readable(this);
            }
        }

        switch(_read_body_type)
        {
        case read_body_to_content_length:
        case read_body_to_thunked_length:
        case read_body_to_connection_close:
            eio::socket_io_filter::notify_readable(this);
            break;
        }
    }

    void http_socket::notify_writeable(ref_class_i* fire_from_handle)
    {
        if(_head_cache_out.size())
        {
            if( _next_socket_io->write_buffer(_head_cache_out ) <= 0 )return;
            _head_cache_out.clear();
        }

        eio::socket_io_filter::notify_writeable(fire_from_handle);
    }

    int http_socket::get_nread_size() const 
    {
        if(!_is_http_opened)return 0;

        int max_bytes=0;

        if( _current_http_parser->has_content_length())
        {
             max_bytes = content_nread;
        }
        int result = _next_socket_io->get_nread_size();
        result += (_head_cache_in.size()-_head_cache_in_offset);
        if(result>max_bytes)result = max_bytes;
        return result;
    }

    int http_socket::do_read_body(char* data,int len)
    {
        if(!_is_http_opened)return -1;
        if(read_body_need_header==_read_body_type)return 0;
        if(read_body_eof==_read_body_type)return 0;

        int result = 0;

        if(read_body_to_connection_close==_read_body_type)
        {
            return do_read_data(data,len);

        }else if(read_body_to_content_length==_read_body_type)
        {
            len = min(content_nread,len);
            result = do_read_data(data,len);
            if(result>0)content_nread-=result;
            if(0==content_nread)
            {
                _read_body_type = read_body_eof;
                notify_closed(this);
            }
            return result;

        }else if(read_body_need_thunked_header==_read_body_type)
        {
            result = do_fetch_chunk_header();
            if(result<=0)return result;
        }

        assert(read_body_to_thunked_length == _read_body_type);
            
        len = min(content_nread,len);
        result = do_read_data(data,len);
        if(result>0)
        {
            content_nread-=result;
            size_t parsed_size = _current_http_parser->parse( data,result );
            assert(parsed_size==result);
        }
        assert(content_nread>=0);
        if(0==content_nread)
        {
            do_fetch_chunk_end();
            do_fetch_chunk_header();
        }
        return result;
    }

    int http_socket::do_fetch_chunk_header()
    {
        if(!_is_http_opened)return -1;
        if( _read_body_type != read_body_need_thunked_header )return 0;

        char buffer[4];

        while(read_body_need_thunked_header==_read_body_type)
        {
            int n = do_read_data(buffer,1);
            if(n==-1)return -1;
            if(0==n)return 0;

            size_t result = _current_http_parser->parse( buffer,1 );
            assert(result==1);
            if(0==result)return -1;
            if(read_body_to_thunked_length==_read_body_type && content_nread )return 1;
        }

        if(_read_body_type == read_body_to_thunked_length )
        {
            do_fetch_chunk_end();
        }

        assert( read_body_eof == _read_body_type );

        if(read_body_eof == _read_body_type)
        {
            notify_closed(this);
        }
        //process-tail
        return 0;
    }

    int http_socket::do_fetch_chunk_end()
    {
        if(!_is_http_opened)return -1;
        if( _read_body_type != read_body_to_thunked_length )return 0;

        char buffer[4];

        while(read_body_to_thunked_length==_read_body_type)
        {
            int n = do_read_data(buffer,1);
            if(n==-1)return -1;
            if(0==n)return 0;

            size_t result = _current_http_parser->parse( buffer,1 );
            assert(result==1);
            if(0==result)return -1;
            if(read_body_to_thunked_length!=_read_body_type)return 1;
        }

        return 0;
    }

    int http_socket::do_read_data(char* data,int len)
    {
        int total_out=0;

        char *out=(char *)data;

        if(_head_cache_in.size())
        {
            const char *p=(const char *)_head_cache_in.data();
            p += _head_cache_in_offset;
            total_out = _head_cache_in.size()-_head_cache_in_offset;

            if( total_out>len )total_out=len;

            memcpy( out,p,total_out );

            _head_cache_in_offset += (int)total_out;
            assert(_head_cache_in.size()>=_head_cache_in_offset);
            if(_head_cache_in.size()==_head_cache_in_offset)
            {
                _head_cache_in.resize(0);
                _head_cache_in_offset = 0;
            }

            out+=total_out;
            len -= total_out;
        }

        if(0==len)return total_out;

        int result = _next_socket_io->read( out,len );
        if(result<0 && 0==total_out)return -1;

        total_out += result;

        return total_out;
    }

    int http_socket::do_head_cache_in()
    {
        if(_head_cache_in.size())return _head_cache_in.size();

        int capacity = _head_cache_in.capacity();
        if(!capacity)capacity = 1024-ebase::buffer::header_size;
        void* data = _head_cache_in.alloc(capacity);
        _head_cache_in_line_end=0;
        _head_cache_in_offset = 0;

        return _next_socket_io->read_buffer(_head_cache_in);
    }

    int http_socket::do_parse_header()
    {
        if(!_current_http_parser)return -1;
        if(_is_http_opened)return 1;

        do 
        {
            int result=do_head_cache_in();
            if(result<=0)return result;

            const char *p=(const char *)_head_cache_in.data();
            p += _head_cache_in_offset;
            int len = _head_cache_in.size()-_head_cache_in_offset;

            int npos = _head_cache_in.find("\r\n",2,_head_cache_in_offset);
            if(npos>=0)len = npos+2-_head_cache_in_offset;

            size_t parsed = _current_http_parser->parse( p,len );
            if(parsed<len)
            {//出错

            }
            _head_cache_in_offset += (int)parsed;
            assert(_head_cache_in.size()>=_head_cache_in_offset);

            if(_head_cache_in.size()==_head_cache_in_offset)
            {
                _head_cache_in.resize(0);
                _head_cache_in_offset = 0;
            }

            if(_is_http_opened)
            {
                do_fetch_chunk_header();
                eio::socket_io_filter::notify_opened(this);
                return (int)result;
            }
        } while (1);
        return 0;
    }

    bool http_socket::on_http_protocol_headers_complete()
    {
        //http 1.1
        if(_current_http_parser->minor_ver>0)
        {
            if(_current_http_parser->upgrade)
            {
                _read_body_type = read_body_to_connection_close;
            }else if(_current_http_parser->chunked)
            {
                _read_body_type = read_body_need_thunked_header;
            }else if( this->_current_http_parser->has_content_length() )
            {
                _read_body_type = read_body_to_content_length;
                content_nread = this->_current_http_parser->get_content_length();
                if(0==content_nread)_read_body_type = read_body_eof;
            }else
            {
                _read_body_type = read_body_to_connection_close;
            }
        }else//http 1.0
        {
            if( this->_current_http_parser->has_content_length() )
            {
                _read_body_type = read_body_to_content_length;
                content_nread = this->_current_http_parser->get_content_length();
            }else
            {
                _read_body_type = read_body_to_connection_close;
            }
        }

        _is_http_opened=true;

        if(read_body_to_content_length==_read_body_type)return false;

        return true;
    }

    void http_socket::on_http_protocol_chunk_header()
    {
        assert(read_body_need_thunked_header==_read_body_type);
        _read_body_type = read_body_to_thunked_length;
        content_nread = this->_current_http_parser->get_content_length();
    }

    void http_socket::on_http_protocol_chunk_complete()
    {
        _read_body_type = read_body_need_thunked_header;
    }

    void http_socket::on_http_protocol_complete()
    {
        if(read_body_to_content_length!=_read_body_type)_read_body_type = read_body_eof;
    }

};