#pragma once
#include "ref_class.hpp"
namespace ebase
{

	class ref_list
	{
	public:
		ref_list();
		~ref_list();

        class entry;

        class pair
        {
        public:
            pair():entry(0){}

            class entry*    entry;
            ref_ptr<>       holder;
        };

		entry*			front() const;
		entry*			back() const;

		bool			push_front( entry* e);
		bool			push_back( entry* e);

		pair		    pop_front();//return holder
		pair		    pop_back();//return holder

		bool			remove(entry* e);
		void			clear();

		inline int		size() {return _count;}

		inline entry*	begin()const {return front();}
		inline entry*	end() const{return 0;}

		class entry
		{
		public:
            entry();

			inline bool				in_list(){return this->list_host!=0;}
            inline void			    set_holder(ref_class_i* holder){_holder=holder;}
			inline ref_class_i*		get_holder(){return _holder;}
			template<class type>
			type*       		    get_holder(){if(!_holder)return 0;return (type*)_holder;}

			entry*					next() const{return _next;}

			virtual void			on_list_insert(ref_list* list);
            virtual void			on_list_remove(ref_list* list);
        private:
            friend class ref_list;
            ref_class_i*        _holder;
            ref_list*			list_host;

			entry*				_prev;
			entry*		        _next;
		};

	private:
		entry*			_first;
		entry*			_last;
		int			    _count;
	};
};