#pragma once
#include "ref_memory.hpp"
//ע�⣬bufferΪcopy-on-write���ƣ�������������bufferͬʱ����ͬһ��buffer::header���ݣ���ô���������assign,append,resize,���������޸ĺ�����buffer::header���ᱻ����һ��
//��buffer��buffer_list��ʱ���������assign,append,resize,���������޸ĺ�����buffer::header���ᱻ����һ��

namespace ebase
{
/**@brief copy-on-write�����ڴ滺����

���������ü���ʵ��copy-on-write���ƣ����buffer���Գ���ͬһ���ڴ�,������һ��buffer����malloc,resize�Ⱥ�������д����ʱ���ڲ������·���һ�����ڴ档
 
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

		void*		resize(int data_size,bool keep_data=true);//���ؿ�д������,resize(0)������ɾ����������ֻ�ǽ����ݳ�������Ϊ0����
        void*       alloc(int capacity);
        void*       grown( int grown_capacity = 0 );
        void        cut_data(int cut_size);//������ǰ������ݣ����������������ǰ�ƶ�

		void		clear();

		const void* data() const;
		int			size() const;
		int			capacity() const;
	
		bool		is_empty() const;

        int         find( const void* data,int len,int startpos=0 ) const;
	private:
		friend class buffer_list;
		header*		probe_unlist_header() const;//����һ�ݲ���list�е����ݣ����б��룬����һ���µ�header�����򷵻�ԭ���ľ���add_ref��header,ʹ������Ժ�ǵ�release
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