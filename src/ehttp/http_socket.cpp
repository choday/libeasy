#include "http_socket.hpp"
#include "../eio/ssl/ssl_socket.hpp"
#include "../estream/zlib_stream.hpp"

#include "stdio.h"
namespace ehttp
{
    http_socket::http_socket()
    {

        _is_write_end=false;
        _head_cache_in_offset=0;

        _is_server=false;
        _current_http_parser=&response;
        _current_http_writer=&request;

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
            _current_http_writer=&response;
        }else
        {
            _current_http_parser=&response;
            _current_http_writer=&request;

            if(!_next_socket_io.valid()&&request.url.schema=="https")
            {
                eio::ssl_socket* s = new eio::ssl_socket();
                s->ssl_set_host_name(request.url.host.c_str());
                socket_io_filter::attach_socket_io(s);
            }
        }

        if(is_server&&_next_socket_io.valid()&&_next_socket_io->is_opened())
        {
            notify_readable(this);
        }
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
/**
@brief 关闭http socket
@details 断开tcp连接，并关闭底层socket，此函数会先中止http协议，将数据发送成以后再关闭
@param[in] delay 延迟到数据发送完毕再关闭
@note 参数delay为true时，立即丢弃所有数据，并关闭socket
*/
    void http_socket::close(bool delay/*=true*/)
    {
        if(delay)
        {
            end_write();
            _current_http_writer->keep_alive=false;
            if(_head_cache_out.size()>0)return;
        }

        _next_socket_io->close(delay);
    }

    int http_socket::read(void* data,int len)
    {
        return read_http_data(data,len);
    }

    int http_socket::write(const void* data,int len)
    {
        return write_http_data(data,len);
        
    }
/**
@brief 开始http协议交互，发送http头部，为后续写入http body数据作准备
*/
    int http_socket::begin_write()
    {
        assert(_head_cache_out.size());

        _head_cache_out = _current_http_writer->make_buffer();

        int result = eio::socket_io_filter::_next_socket_io->write_buffer(_head_cache_out);
        if(result>0)_head_cache_out.clear();
        
        return result;
    }
/**
@brief 结束http协议交互，如有需要会发送chunk end
*/
    bool http_socket::end_write()
    {
        if(_is_write_end)return true;
        _is_write_end=true;
        if(_current_http_writer->chunked)do_write_http_chunk(0,0);
        if(_head_cache_out.size()>0)
        {
            int n = _next_socket_io->write( _head_cache_out.data(),_head_cache_out.size() );
            if(n<=0)return true;
            _head_cache_out.resize(0);
        }

        return true;
    }


    void http_socket::notify_error(ref_class_i* fire_from_handle)
    {
        eio::socket_io_filter::notify_error(fire_from_handle);
    }

    int http_socket::send_response(const ebase::string& data/*=""*/)
    {
        response.chunked = false;
        response.content_length = data.size();

        if(begin_write()<=0)return -1;
        if(data.size())
        {
            int result = write_http_data( data.data(),data.size() );
            if(result<0)return -1;
        }
        end_write();
        return data.size()+1;
    }


    int http_socket::send_response(int status_code/*=200*/,const ebase::string& status_msg/*=""*/)
    {
        response.chunked=false;
        response.content_length = 0;
        response.keep_alive = false;
        response.status_code = status_code;
        
        if(status_msg.length())
            response.status_message = status_msg;
        else
            response.status_message = http_status_str((http_status)status_code);

        if(begin_write()<=0)return -1;

        end_write();
        return 1;
    }

    void http_socket::notify_opened(ref_class_i* fire_from_handle)
    {
        if(!_is_server)_head_cache_out = this->request.make_buffer();
    }

    void http_socket::notify_closed(ref_class_i* fire_from_handle)
    {
        eio::socket_io_filter::notify_closed(fire_from_handle);
    }

    void http_socket::notify_writeable(ref_class_i* fire_from_handle)
    {
        if(_head_cache_out.size())
        {
            if( _next_socket_io->write_buffer(_head_cache_out ) <= 0 )return;
            _head_cache_out.clear();
        }

        if( _is_write_end  )
        {
            if(!_current_http_writer->keep_alive && 0==_head_cache_out.size())
            {
                _next_socket_io->close();
            }
            return;
        }

        eio::socket_io_filter::notify_writeable(fire_from_handle);
    }

    void http_socket::notify_readable(ref_class_i* fire_from_handle)
    {
        while (!_current_http_parser->is_header_completed())
        {
            if(!_head_cache_in.size())
            {
                int capacity = _head_cache_in.capacity();
                if(!capacity)capacity = 1024-ebase::buffer::header_size;
                void* data = _head_cache_in.alloc(capacity);
                _head_cache_in_offset = 0;

                int result = _next_socket_io->read_buffer(_head_cache_in);
                if(result<=0)return;
            }

            const char *p=(const char *)_head_cache_in.data();
            p += _head_cache_in_offset;
            int len = _head_cache_in.size()-_head_cache_in_offset;

            size_t parsed = _current_http_parser->parse_http( p,len );
            assert(parsed>0);

            if(0==parsed)
            {//出错
                notify_error(this);
            }

            _head_cache_in_offset += (int)parsed;
            assert(_head_cache_in.size()>=_head_cache_in_offset);

            if(_head_cache_in.size()==_head_cache_in_offset)
            {
                _head_cache_in.resize(0);
                _head_cache_in_offset = 0;
            }
        }

        if(_is_server)
        {
            _current_http_writer->keep_alive = _current_http_parser->keep_alive;
        }

        if(_current_http_parser->is_parse_completed())return;

        do_skip_chunked();

        eio::socket_io_filter::notify_readable(this);
    }

    int http_socket::do_skip_chunked()
    {
        if(!_current_http_parser->is_header_completed())return -1;
        if(_current_http_parser->is_parse_completed())return 0;
        if(false==_current_http_parser->chunked)return 0;

        char buffer[4];

        int total_size = 0;
        while(_current_http_parser->body_length==0 && !_current_http_parser->is_parse_completed() )
        {
            int n = do_read_socket_data(buffer,1);
            if(n<=0)return n;

            size_t result = _current_http_parser->parse_http( buffer,1 );
            assert(result==1);
            if(0==result)return -1;

            total_size++;
        }
        if(_current_http_parser->is_parse_completed())return -1;

        return _current_http_parser->body_length;
    }

    int http_socket::read_http_data(void* data,int len)
    {
        int result = -1;
        int total_size = 0;

        char* p=(char*)data;

        total_size=this->do_read_http_body( p,len );
        if(total_size<=0)return total_size;

        p += total_size;
        len -= total_size;

        while(len>0) 
        {
            result=this->do_read_http_body( p,len );
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

    int http_socket::do_read_http_body(char* data,int len)
    {
        if(0==len)return 0;

        if(0==_current_http_parser->body_length)
        {
            if(_current_http_parser->is_parse_completed())return -1;//eof
            int n=do_skip_chunked();
            if(n<0)return n;
        }

        if(-1==_current_http_parser->body_length)return do_read_socket_data(data,len);
        if(len>_current_http_parser->body_length)len=_current_http_parser->body_length;

        int result = do_read_socket_data(data,len);
        if(result<=0)return result;

        size_t parsed = _current_http_parser->parse_http( data,result );
        assert(parsed==result);

        if(0==_current_http_parser->body_length)
        {
            do_skip_chunked();
        }

        if(_current_http_parser->is_parse_completed())
        {//eof
            assert(0==_current_http_parser->body_length);
            notify_closed(this);
        }
        return result;
    }

    int http_socket::do_read_socket_data(char* data,int len)
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
/**
@brief 获取缓存的数据长度
@attention 返回值并非准确可读长度，而是底层socket未读长度与http层已缓存的数据长度
*/
    int http_socket::get_nread_size() const 
    {
        int result = _next_socket_io->get_nread_size();
        result += (_head_cache_in.size()-_head_cache_in_offset);
        if(result>_current_http_parser->body_length)result = _current_http_parser->body_length;
        return result;
    }

    int http_socket::get_error_code() const 
    {
        int code =_current_http_parser->get_error_code();
        if(0==code )code=_next_socket_io->get_error_code();
        return code;
    }

    const char* http_socket::get_error_message() const 
    {
        int code =_current_http_parser->get_error_code();
        if(0==code )return _next_socket_io->get_error_message();

        return _current_http_parser->get_error_message();
    }

    int http_socket::write_http_data(const void* data,int len)
    {
        if(0==len)return 0;
        if(_is_write_end)return -1;
        if(_head_cache_out.size()>0)
        {
            int n = _next_socket_io->write( _head_cache_out.data(),_head_cache_out.size() );
            if(n<=0)return n;
            _head_cache_out.resize(0);
        }

        if(_current_http_writer->chunked||_current_http_writer->upgrade)return do_write_http_chunk(data,len);

        if(0==_current_http_writer->content_length)return -1;

        if(-1==_current_http_writer->content_length)return _next_socket_io->write(data,len);

        if(len>_current_http_writer->content_length)len = _current_http_writer->content_length;

        int n = _next_socket_io->write(data,len);
        if(n<=0)return n;

        _current_http_writer->content_length-=n;
        if(0==_current_http_writer->content_length)_is_write_end=true;
        return n;
    }

    int http_socket::do_write_http_chunk(const void* data,int len)
    {
        if(_head_cache_out.size()>0)
        {
            int n = _next_socket_io->write( _head_cache_out.data(),_head_cache_out.size() );
            if(n<=0)return n;
            _head_cache_out.resize(0);
        }
        
        ebase::string chunk_header;
        chunk_header.fomart_assign("%x\r\n",len );

        int total_size = 0;

        int result = _next_socket_io->write( chunk_header.data(),chunk_header.length() );
        if(result<=0)return result;

        total_size+=result;

        if(len)
        {
            result = _next_socket_io->write(data,len);
            if( -1 == result )return -1;//出错
            if(0==result)
            {//cache
                _head_cache_out.assign(data,len);
                _head_cache_out.append("\r\n",2);
                return total_size;
            }
            assert(result==len);
            total_size+=result;
        }
        
        result = _next_socket_io->write("\r\n",2);
        if(result<=0)
        {
            _head_cache_out.assign( "\r\n",2);
        }else
        {
            total_size+=result;
        }
        
        return total_size;
    }
};