#pragma once
#include "../ebase/ref_class.hpp"
#include "../ebase/buffer.hpp"
#include "../ebase/event_emitter.hpp"

namespace eio
{

    class io_method:public ebase::ref_class_i
    {
    public:
        io_method();

		ebase::event_emitter				on_error;///< io����,����get_error_code,get_error_message��ȡ������Ϣ
		ebase::event_emitter				on_closed;///< io���󱻹ر�
		ebase::event_emitter				on_readable;///< �ɽ��ж�����
		ebase::event_emitter				on_writeable;///< ��д

        ///Ϊ�����¼�����ִ����ebase::executor
        virtual void            set_event_executor( ebase::executor* event_executor );

        
        virtual bool            is_opened() = 0;///< io�Ƿ��
        virtual int             get_error_code() const = 0;///< ��ȡ������
        virtual const char*     get_error_message() const =0;///< ��ȡ�����������ַ���

/**
@brief �رն���

@params[in] delay �Ƿ��ӳٹر�
@note �������delay=true����ô��ȵ��������ݴ�����ղŻ������ر�,���������رղ�������������
*/
        virtual void            close(bool delay=true) = 0;
/**
@brief ��ȡ���� 

@param[out] data ������
@param[in] len ����������
@retval >0 �������ݳ���
@retval 0 �����ݿɶ�
@retval <0 �����Ժ��ٿɶ�,�������get_error_code
*/
        virtual int             read(void* data,int len) = 0;
/**
@brief д������

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
@brief ��ȡ���ݵ�ebase::buffer����
@details ���²�ֱ�ӵ�������,����²�û��ʵ�֣�Ĭ�ϵ���read
@param[out] data ���ص����ݻ��������������ǰ���䣬��ײ��Զ�����
@return ���ص������ݳ���,��data.size();������-1,����д����0
@note �˺������������������ײ�ʵ���ߣ��������޷����ƻ����С,��������data.capacity()==0,�ײ����data.alloc()�����ڴ�
*/
        virtual int            read_buffer( ebase::buffer& data );
/**
@brief д������ 
@details ��һ�����ݿ黺����ֱ���͸��²㴦��,�������ݸ���,����²�û�д���Ĭ�ϵ���write
@param[in] data
@return �������ݳ���,��data.size();������-1,����д����0
@note �˺������������������ײ�ʵ���ߣ��������޷����ƻ����С
*/
        virtual int            write_buffer( const ebase::buffer& data );


    };

};