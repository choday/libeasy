#pragma once

namespace ebase
{
	struct ref_memory
	{
		struct header
		{
			long	ref_count;
            int     total_size;

			static	header* alloc(int size);//default:ref_count=1
			header*			realloc( int size );

			void	init(int size);
			long	add_ref();
			long	release(bool auto_free=true);
		};

		void	init(struct header* p=0);
		void	clear();
		void	assign(struct header* p);
		void	assign(const struct ref_memory& p);
		bool	is_empty() const;

		/*
		����ԭ�����ݣ��������µ�header
		����ָ��header��ָ�룬��_data������,ʹ������Ժ󣬲�Ҫ����release

		size����ref_memory::header����
		*/
		void*	alloc(int size );
		/*
		������С������write-on-copy���ƣ����header��������ã��������µ�header,���Ҹ��� keep_data_size���ݵ��µ�header
		����ָ��header��ָ�룬��_data������,ʹ������Ժ󣬲�Ҫ����release

		keep_data_size=0ʱ�����Ե���allocʹ��
		*/
		void*	resize(int new_size,int keep_data_size=0);

		struct header*	_data;
	};

	template<typename header_name>
	struct ref_memory_t
	{
#define as_ref_memory() ((ref_memory*)this)
		void	init(header_name* p=0){ as_ref_memory()->init(p);}
		void	clear(){ as_ref_memory()->clear();}
		void	assign(struct ref_memory::header* p){ as_ref_memory()->assign(p);}
		void	assign(const struct ref_memory_t& p){ as_ref_memory()->assign(p._data);}
		void*	alloc(int capacity_size){ return as_ref_memory()->alloc(capacity_size);}
		void*	resize(int new_size,int keep_data_size=0){ return as_ref_memory()->resize(new_size,keep_data_size);}
		bool	is_empty() const{return as_ref_memory()->is_empty();}
		header_name*	_data;
	};


}