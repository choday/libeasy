#pragma once
#include "ref_memory.hpp"
//注意，buffer为copy-on-write机制，如果两个及多个buffer同时持有同一份buffer::header内容，那么，如果调用assign,append,resize,三个内容修改函数，buffer::header将会被复制一份
//当buffer在buffer_list中时，如果调用assign,append,resize,三个内容修改函数，buffer::header将会被复制一份

namespace ebase
{
/**@brief copy-on-write机制内存缓存区

本类用引用计数实现copy-on-write机制，多个buffer可以持有同一份内存,当其中一个buffer调用malloc,resize等函数进行写操作时，内部分重新分配一份新内存。
 
*/
	class buffer
	{
	public:
		struct header:public ref_memory::header
		{
			struct header*	_prev;
			struct header*	_next;
			int				size;
			char			data[sizeof(void*)];
		};

        static const int    header_size;

		buffer();
		buffer(const buffer& v);
		buffer( header* p );
		~buffer();
		buffer& operator=(const buffer& v);

		buffer&	assign(const buffer& v);
		buffer&	assign(const void* data,int size);

		buffer&	append(const buffer& v);
		buffer&	append(const void* data,int size);

		void*		resize(int data_size,bool keep_data=true);//返回可写缓存区,resize(0)并不且删除缓冲区，只是将数据长度设置为0而已
        void*       alloc(int capacity);
        void*       grown( int grown_capacity = 0 );
        void        cut_data(int cut_size);//剪掉掉前面的数据，后面的数据依次向前移动

		void		clear();

		const void* data() const;
		int			size() const;
		int			capacity() const;
	
		bool		is_empty() const;

        int         find( const void* data,int len,int startpos=0 ) const;
	private:
		friend class buffer_list;
		header*		probe_unlist_header() const;//返回一份不在list中的内容，如有必须，复制一份新的header，否则返回原来的经过add_ref的header,使用完成以后记得release
		buffer		next();

	private:
		
        ref_memory_t<buffer::header> _data;
	};


	class buffer_list
	{
	public:
		buffer_list();
		buffer_list(const buffer_list& v);
		~buffer_list();

		buffer_list&	assign(const buffer_list& v);
		buffer_list&	operator=(const buffer_list& v);
		void	push_back( const buffer& input );
		void	push_front( buffer& input );
		buffer	pop_front();
		buffer	pop_back();
		void	clear();

		buffer front() const;
		buffer back() const;

		int		count() const;

		int		data_size() const;
	private:
		int				_data_size;
		int				_count;
		buffer::header* _first;
		buffer::header* _last;
	};
};