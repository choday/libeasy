#pragma once
#include "../ebase/ref_class.hpp"
#include "../ebase/executor.hpp"
#include "../ebase/event_emitter.hpp"
#include "../ebase/string.hpp"
#include "../ebase/buffer.hpp"
#include "socket_address.hpp"
namespace eio
{
    class socket_io:public ebase::ref_class_i
    {
    public:
 
        ///tcp�������ɹ���
		ebase::event_emitter				on_opened;
        ///socket����
		ebase::event_emitter				on_error;
        ///tcp���������ر�
		ebase::event_emitter				on_closed;

        ///�ɽ��ж�����,���߿ɽ���accept����
		ebase::event_emitter				on_readable;
        ///��д
		ebase::event_emitter				on_writeable;

        ///Ϊ�����¼�����ִ����ebase::executor
        virtual void            set_event_executor( ebase::executor* event_executor );
        virtual void            clear_all_event();
/**@brief ��һ��������,�ײ����tcp socket connectϵͳ����ʵ��

@param[in] host Ŀ����������ip�ַ���
@param[in] port_or_service Ŀ��port����service�ַ�������"80"��"http"��
@param[in] address Ŀ��inet sockaddr
@return �ɹ�����true
@note ����true,��ζ�ųɹ�����connect,���ӳɹ�,on_opened�¼����ᱻ����
*/
		virtual bool			open( const ebase::string& host,const ebase::string& port_or_service ) =0;
		virtual bool			open(const socket_address& address ) =0;
        ///tcp�������Ƿ��
        virtual bool            is_opened() = 0;
/**
@brief �ر��׽��� 

@params[in] delay �Ƿ��ӳٹر�
@note �������delay=true����ô��ȵ��������ݷ�����ղŻ������ر�socket,���������ر�socket������������
*/
        virtual void            close(bool delay=true) = 0;
/**
@brief ��ȡ�׽����е����� 

@param[out] data ������
@param[in] len ����������
@retval >0 �������ݳ���
@retval 0 �����ݿɶ�
@retval <0 �����Ժ��ٿɶ�,�������get_error_code
*/
        virtual int             read(void* data,int len) = 0;
/**
@brief д�����ݵ��׽���

@param[in] data ������
@param[in] len ����������
@retval >0 �������ݳ���
@retval 0 �����ݿɶ�
@retval <0 �����Ժ��ٿ�д,�������get_error_code
@note ����д�������Ҫôȫ��д�룬Ҫô��д�룬������д�벿�������Ĳ���,���ԣ�����ֵ�������-1,0,��һ����len
@attention ע�⣬�������len=0,��ô����ֵҲ��Ϊ0
*/
        virtual int             write(const void* data,int len) = 0;
/**
@brief �������� ,�������ݸ���
@details �ӵײ�ֱ�ӵ�������
@param[out] data ���ص����ݻ��������������ǰ���䣬��ײ��Զ�����
@return ���ص������ݳ���,��data.size();������-1,����д����0
@note �˺������������������ײ�ʵ���ߣ��������޷����ƻ����С,��������data.capacity()==0,�ײ����data.alloc()�����ڴ�
*/
        virtual int            read_buffer( ebase::buffer& data ) = 0;
/**
@brief ѹ������ 
@details ��һ�����ݿ黺����ֱ���͸��²㴦��,�������ݸ���
@param[in] data
@return �������ݳ���,��data.size();������-1,����д����0
@note �˺������������������ײ�ʵ���ߣ��������޷����ƻ����С
@note �������false,����get_error_code()==0��socket���ر�,���ߴ���data.size()=0,����is_opened�鿴socket�Ƿ񱻹ر�
*/
        virtual int            write_buffer( const ebase::buffer& data ) = 0;
/**
@brief �鿴����������������ݳ���

*/
        virtual int             get_nread_size() const =0;
        virtual int             get_error_code() const = 0;
        virtual const char*     get_error_message() const =0;
    };

    typedef ebase::ref_ptr<socket_io>   socket_io_ptr;

    class socket_io_wrap:public ebase::ref_class<socket_io>
    {
    public:
        socket_io_wrap();
        ~socket_io_wrap();

        
        virtual void            attach_socket_io(socket_io* next);
        socket_io*              get_next_socket_io();

        virtual void            set_event_executor( ebase::executor* event_executor ) override;

		virtual bool			open( const ebase::string& host,const ebase::string& port_or_service ) override;
		virtual bool			open(const socket_address& address ) override;
        virtual bool            is_opened() override;
        virtual void            close(bool delay=true) override;
          
        virtual int             read(void* data,int len) override;
        virtual int             write(const void* data,int len) override;

        virtual int             read_buffer( ebase::buffer& data ) override;
        virtual int             write_buffer( const ebase::buffer& data ) override;

        virtual int             get_nread_size() const override;
        virtual int             get_error_code() const override ;
        virtual const char*     get_error_message() const override;
    protected:
		virtual void			notify_error(ref_class_i* fire_from_handle);
		virtual void			notify_opened(ref_class_i* fire_from_handle);
		virtual void			notify_closed(ref_class_i* fire_from_handle);

		virtual void			notify_readable(ref_class_i* fire_from_handle);
		virtual void			notify_writeable(ref_class_i* fire_from_handle);

        ebase::ref_ptr<socket_io>   _next_socket_io;
        ebase::executor*            _event_executor;
    };


};