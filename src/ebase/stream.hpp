#pragma once
#include "buffer.hpp"
#include "event_emitter.hpp"
#include "error.hpp"
#include "thread.hpp"
namespace ebase
{
	class stream_writeable:public virtual ref_class<>
	{
	public:
		stream_writeable();

		void					set_event_executor( executor* event_executor );

		virtual bool			write(const buffer& data,bool flush_now=false);
		bool					flush();
		bool					end();//结束可写缓冲区，调用后不再可用,private_on_want_write将会触发
		bool					is_end();
        int                     get_write_cache_size();

		event_emitter			on_drain;//缓冲区为空
		event_emitter			on_error;
        ebase::error            error;

	public://实现者访问
		buffer					private_pop_buffer();
        void					private_return_buffer(ebase::buffer& buffer);//用于回退buffer,如果向底层发送失败，可以使用此函数将buffer退回到缓存

		void					private_set_event_executor( executor* event_executor );
		event_emitter			private_on_want_pop_buffer;//实现者继承,flush,end
	private:
        int                         _write_cache_size;
		bool						_is_end;
		mutex_lock_t<buffer_list>	_buffer_list;
	};

	class stream_readable:public virtual ref_class<>
	{
	public:
		stream_readable();

		void            set_event_executor( executor* event_executor );

		bool			read(buffer& data);
		bool			pause();//暂停，on_data将不会调用
		bool			resume();//恢复，如果有数据，on_data将会被调用
		bool			is_paused();
		bool			pip(stream_writeable* writeable);

        int				get_nread_size();
        inline void     set_read_cache(int size=4096){_read_cache_size=size;}

		event_emitter	on_data;//有数据可读
		event_emitter	on_end;//可读缓冲区到末尾，以后不再可用
        ebase::error    error;

	public://实现者访问
		void            private_push_buffer(const buffer& data);
		void            private_notify_end();

		void			private_set_event_executor( executor* event_executor );
		event_emitter	private_on_want_push_buffer;//实现者继承,pause,resume,read(empty)
	private:
        int                         _read_cache_size;
		mutex_lock_t<buffer_list>	_buffer_list;
        bool                        _is_eof;
		bool						_is_paused;
	};

	class stream:public stream_readable,public stream_writeable
	{
	public:
		stream();

        void            set_event_executor( executor* event_executor );

        event_emitter   on_opened;
        ebase::error    error;
    public:
        void			private_set_event_executor( executor* event_executor );
        void            private_notify_error(int code,const char* msg=0);
        void            private_notify_error(const ebase::error& e);
	};
}