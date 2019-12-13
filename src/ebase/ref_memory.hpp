#pragma once

namespace ebase
{
/**@brief �����ü������ڴ�

�����࣬��������ʹ��,�ܱ�ʾ����ڴ�Ϊ2G
*/
	struct ref_memory
	{
		struct header
		{
            ///���ü�������������Ϊ0ʱ�����ڴ潫�ᱻ�ͷ�
			long	ref_count;
            ///�ڴ��ܴ�С,����headerռ���ֽ�����
            int     total_size;

			static	header* alloc(int size);//default:ref_count=1
			void*			realloc( int size );

			///���ü����Ƿ�Ϊ1
            bool    is_alone();
			long	add_ref();
			long	release(bool auto_free=true);

        private:
            void	init(int size);
		};

		void	init(struct header* p=0);
		void	clear();
		void	assign(struct header* p);
		void	assign(const struct ref_memory& p);
		bool	is_empty() const;

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
		void*	resize(int new_size,int keep_data_size=0){ return as_ref_memory()->resize(new_size,keep_data_size);}
		bool	is_empty() const{return as_ref_memory()->is_empty();}
		header_name*	_data;
	};


}