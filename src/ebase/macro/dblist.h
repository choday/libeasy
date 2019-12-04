#include <assert.h>
/*
head:_first,_last
entry:_prev,_next
第一个元素_prev第向自己
*/

#define DBLIST_NULL_PTR 0


//define getter and setter
#ifndef _DBLIST_FIRST_SETTER
#define _DBLIST_FIRST_SETTER( head,entry ) ((head)->_first=entry)
#define _DBLIST_FIRST_GETTER( head ) ((head)->_first)

#define _DBLIST_LAST_SETTER( head,entry ) ((head)->_last=entry)
#define _DBLIST_LAST_GETTER( head ) ((head)->_last)

#define _DBLIST_PREV_SETTER( entry,entry_new ) ((entry)->_prev=entry_new)
#define _DBLIST_PREV_GETTER( entry ) ((entry)->_prev)

#define _DBLIST_NEXT_SETTER( entry,entry_new ) ((entry)->_next=entry_new)
#define _DBLIST_NEXT_GETTER( entry ) ((entry)->_next )
#endif

#define DBLIST_IS_IN_LIST(entry) (_DBLIST_PREV_GETTER(entry)!=DBLIST_NULL_PTR)
#define DBLIST_IS_FIRST_ENTRY(entry) (_DBLIST_PREV_GETTER(entry)==entry)
#define DBLIST_IS_LAST_ENTRY(entry) (_DBLIST_NEXT_GETTER(entry)==DBLIST_NULL_PTR)
#define DBLIST_IS_EMPTY(head)	(_DBLIST_FIRST_GETTER(head)==DBLIST_NULL_PTR)

#define DBLIST_ENTRY_INIT( _entry )\
	_DBLIST_PREV_SETTER(_entry,DBLIST_NULL_PTR);\
	_DBLIST_NEXT_SETTER(_entry,DBLIST_NULL_PTR)

#define DBLIST_HEAD_INIT( head )\
	_DBLIST_FIRST_SETTER(head,DBLIST_NULL_PTR);\
	_DBLIST_LAST_SETTER(head,DBLIST_NULL_PTR) 
/*
DBLIST_INSERT_AFTER
	if(DBLIST_IS_EMPTY(head) || (entry==DBLIST_NULL_PTR) )
	{
		DBLIST_PUSH_FRONT(head,entry_new);
	}else if(head.last==entry)
	{
		DBLIST_PUSH_BACK(head,entry_new);
	}else
	{
		entry_new.prev=entry;
		entry_new.next=entry.next;

		entry.next.prev=entry_new;
		entry.next=entry_new;
	}
*/
//insert entry_new after entry
#define DBLIST_INSERT_AFTER( head,entry,entry_new )\
{\
	if(DBLIST_IS_EMPTY(head) || entry ==DBLIST_NULL_PTR )\
	{\
		DBLIST_PUSH_FRONT(head,entry_new);\
	}else if(_DBLIST_LAST_GETTER(head) == _DBLIST_FIRST_GETTER(head) || _DBLIST_LAST_GETTER(head)==entry )\
	{\
		DBLIST_PUSH_BACK(head,entry_new);\
	}else\
	{\
		_DBLIST_PREV_SETTER( entry_new,entry );\
		_DBLIST_NEXT_SETTER( entry_new,_DBLIST_NEXT_GETTER(entry) );\
\
		_DBLIST_PREV_SETTER( _DBLIST_NEXT_GETTER(entry),entry_new );\
		_DBLIST_NEXT_SETTER( entry,entry_new );\
	}\
}\
/*
DBLIST_ERASE
	if(DBLIST_IS_EMPTY(head))
	{

	}else if( _DBLIST_FIRST_GETTER(head) == entry )
	{
		DBLIST_POP_FRONT(head);
	}else if(_DBLIST_LAST_GETTER(head)==entry)
	{
		DBLIST_POP_BACK(head);
	}else
	{
		entry.prev.next = entry.next;
		entry.next.prev = entry.prev;
	}
*/
//remove entry from head
#define DBLIST_ERASE( head,entry )\
{\
\
	if(DBLIST_IS_EMPTY(head))\
	{\
	}else if( _DBLIST_FIRST_GETTER(head) == entry ) \
	{ \
		DBLIST_POP_FRONT(head);\
	}else if( _DBLIST_LAST_GETTER(head)==entry)\
	{\
		DBLIST_POP_BACK(head);\
	}else\
	{\
		_DBLIST_PREV_SETTER(_DBLIST_NEXT_GETTER(entry),_DBLIST_PREV_GETTER(entry));/*entry.next.prev = entry.prev*/\
		_DBLIST_NEXT_SETTER(_DBLIST_PREV_GETTER(entry),_DBLIST_NEXT_GETTER(entry) );/*entry.prev.next = entry.next*/\
\
		_DBLIST_PREV_SETTER( entry,DBLIST_NULL_PTR );\
		_DBLIST_NEXT_SETTER( entry,DBLIST_NULL_PTR );\
	}	\
}

/*DBLIST_PUSH_FRONT
entry.prev=entry
if(head.first==null)
{
	head.last=entry;
}else
{
	entry.next = head.first;
	head.first.prev=entry;
}
head.first=entry;
*/
#define DBLIST_PUSH_FRONT( head,entry ) \
{\
	_DBLIST_PREV_SETTER( entry,entry );\
	if(DBLIST_IS_EMPTY(head))\
	{\
		_DBLIST_LAST_SETTER(head,entry );\
	}else\
	{\
		_DBLIST_NEXT_SETTER( entry,_DBLIST_FIRST_GETTER(head) );\
		_DBLIST_PREV_SETTER( _DBLIST_FIRST_GETTER(head),entry );\
	}\
	_DBLIST_FIRST_SETTER(head,entry );\
}
/*
DBLIST_PUSH_BACK

if(DBLIST_IS_EMPTY(head))
{
	entry.prev=entry;
	head.first=entry;
}else
{
	entry.prev=head.last;
	head.last.next=entry;
}
head.last=entry
*/
#define DBLIST_PUSH_BACK( head,entry )\
{\
	if(DBLIST_IS_EMPTY(head))\
	{\
		_DBLIST_PREV_SETTER( entry,entry );\
		_DBLIST_FIRST_SETTER(head,entry );\
	}else\
	{\
		_DBLIST_PREV_SETTER( entry,_DBLIST_LAST_GETTER(head) );\
		_DBLIST_NEXT_SETTER( _DBLIST_LAST_GETTER(head),entry );\
	}\
	_DBLIST_LAST_SETTER(head,entry );\
}


#define DBLIST_POP_FRONT( head )\
{\
	if( DBLIST_IS_EMPTY(head) )\
	{\
	}else if(_DBLIST_FIRST_GETTER(head) == _DBLIST_LAST_GETTER(head) )\
	{\
		_DBLIST_PREV_SETTER( _DBLIST_FIRST_GETTER(head),DBLIST_NULL_PTR );/*set entry.prev=null*/\
		_DBLIST_FIRST_SETTER(head,DBLIST_NULL_PTR );/*clear,set first=null*/\
		_DBLIST_LAST_SETTER(head,DBLIST_NULL_PTR );/*clear,set last = null*/\
	}else\
	{\
		_DBLIST_FIRST_SETTER(head,_DBLIST_NEXT_GETTER(_DBLIST_FIRST_GETTER(head)) );/*set first = first.next*/\
		_DBLIST_PREV_SETTER( _DBLIST_PREV_GETTER(_DBLIST_FIRST_GETTER(head)),DBLIST_NULL_PTR );/*first.prev.prev=null,first.prev.next=null*/\
		_DBLIST_NEXT_SETTER( _DBLIST_PREV_GETTER(_DBLIST_FIRST_GETTER(head)),DBLIST_NULL_PTR );\
		_DBLIST_PREV_SETTER( _DBLIST_FIRST_GETTER(head),_DBLIST_FIRST_GETTER(head) );/*mark first entry,first.prev=first*/\
	}\
}

#define DBLIST_POP_BACK( head ) \
{\
	if( DBLIST_IS_EMPTY(head) )\
	{\
	}else if(_DBLIST_FIRST_GETTER(head) == _DBLIST_LAST_GETTER(head) )\
	{\
		_DBLIST_PREV_SETTER( _DBLIST_FIRST_GETTER(head),DBLIST_NULL_PTR );/*set entry.prev=null*/\
		_DBLIST_FIRST_SETTER(head,DBLIST_NULL_PTR );/*clear,set first=null*/\
		_DBLIST_LAST_SETTER(head,DBLIST_NULL_PTR );/*clear,set last = null*/\
	}else\
	{\
		_DBLIST_LAST_SETTER( head,_DBLIST_PREV_GETTER(_DBLIST_LAST_GETTER(head)) );/*set last=last.prev*/\
		_DBLIST_PREV_SETTER( _DBLIST_NEXT_GETTER(_DBLIST_LAST_GETTER(head)),DBLIST_NULL_PTR );/*set last.next.prev=null */\
		_DBLIST_NEXT_SETTER( _DBLIST_LAST_GETTER(head),DBLIST_NULL_PTR );/*set last.next=null*/\
	}\
}

/*
static bool test_dblist()
{
	struct entry
	{
		struct entry* _prev;
		struct entry* _next;
	} entrys[5]={0};

	struct  
	{
		struct entry* _first;
		struct entry* _last;
	}head={0};


	DBLIST_PUSH_FRONT( &head,&entrys[0] );
	if(head._first!=&entrys[0])return false;
	if(head._last!=&entrys[0])return false;
	if(entrys[0]._prev!=&entrys[0])return false;
	if(entrys[0]._next!=0 )return false;

	DBLIST_POP_FRONT(&head);
	if(entrys[0]._prev)return false;
	if(entrys[0]._next)return false;
	if(head._last)return false;
	if(head._first)return false;

	DBLIST_PUSH_BACK( &head,&entrys[0] );
	if(head._first!=&entrys[0])return false;
	if(head._last!=&entrys[0])return false;
	if(entrys[0]._prev!=&entrys[0])return false;
	if(entrys[0]._next!=0 )return false;

	DBLIST_POP_BACK(&head);
	if(entrys[0]._prev)return false;
	if(entrys[0]._next)return false;
	if(head._last)return false;
	if(head._first)return false;


	DBLIST_PUSH_BACK( &head,&entrys[0] );
	if(head._first!=&entrys[0])return false;
	if(head._last!=&entrys[0])return false;
	if(entrys[0]._prev!=&entrys[0])return false;
	if(entrys[0]._next!=0 )return false;

		DBLIST_PUSH_BACK( &head,&entrys[1] );
		if(head._first!=&entrys[0])return false;
		if(head._last!=&entrys[1])return false;
		if(entrys[0]._prev!=&entrys[0])return false;
		if(entrys[0]._next!=&entrys[1] )return false;
		if(entrys[1]._prev!=&entrys[0])return false;
		if(entrys[1]._next!=0)return false;

			DBLIST_PUSH_BACK( &head,&entrys[2] );
			if(entrys[0]._prev!=&entrys[0])return false;
			if(entrys[0]._next!=&entrys[1] )return false;
			if(entrys[1]._prev!=&entrys[0])return false;
			if(entrys[1]._next!=&entrys[2] )return false;
			if(entrys[2]._prev!=&entrys[1])return false;
			if(entrys[2]._next!=0 )return false;

				DBLIST_PUSH_FRONT( &head,&entrys[3] );
				if(head._first!=&entrys[3])return false;
				if(entrys[3]._prev!=&entrys[3])return false;
				if(entrys[3]._next!=&entrys[0] )return false;
				if(entrys[0]._prev!=&entrys[3] )return false;

				DBLIST_POP_FRONT(&head);
				if(entrys[0]._prev!=&entrys[0])return false;
				if(entrys[0]._next!=&entrys[1] )return false;
				if(entrys[1]._prev!=&entrys[0])return false;
				if(entrys[1]._next!=&entrys[2] )return false;
				if(entrys[2]._prev!=&entrys[1])return false;
				if(entrys[2]._next!=0 )return false;

			DBLIST_POP_BACK(&head);
			if(head._first!=&entrys[0])return false;
			if(head._last!=&entrys[1])return false;
			if(entrys[0]._prev!=&entrys[0])return false;
			if(entrys[0]._next!=&entrys[1] )return false;
			if(entrys[1]._prev!=&entrys[0])return false;
			if(entrys[1]._next!=0)return false;

		DBLIST_POP_BACK(&head);
		if(head._first!=&entrys[0])return false;
		if(head._last!=&entrys[0])return false;
		if(entrys[0]._prev!=&entrys[0])return false;
		if(entrys[0]._next!=0 )return false;

	DBLIST_POP_FRONT(&head);
	if(!DBLIST_IS_EMPTY(&head))return false;

	DBLIST_PUSH_BACK( &head,&entrys[0] );
	DBLIST_PUSH_BACK( &head,&entrys[1] );
	DBLIST_PUSH_BACK( &head,&entrys[2] );
	DBLIST_ERASE(&head,&entrys[1]);
	if(head._first!=&entrys[0])return false;
	if(head._last!=&entrys[2])return false;
	if( entrys[1]._prev || entrys[1]._next )return false;
	if(entrys[0]._prev != &entrys[0] )return false;
	if( entrys[0]._next != &entrys[2] )return false;
	if( entrys[2]._next )return false;
	if( entrys[2]._prev != &entrys[0] )return false;

	DBLIST_ERASE(&head,&entrys[2]);
	if(head._first!=&entrys[0])return false;
	if(head._last!=&entrys[0])return false;
	if(entrys[0]._prev!=&entrys[0])return false;
	if(entrys[0]._next!=0 )return false;

	DBLIST_ERASE(&head,&entrys[0]);
	if(!DBLIST_IS_EMPTY(&head))return false;

	DBLIST_INSERT_AFTER( &head,(struct entry*)0, &entrys[0] );
	if(head._first!=&entrys[0])return false;
	if(head._last!=&entrys[0])return false;
	if(entrys[0]._prev!=&entrys[0])return false;
	if(entrys[0]._next!=0 )return false;

	DBLIST_INSERT_AFTER( &head,&entrys[0], &entrys[2] );
	if(head._first!=&entrys[0])return false;
	if(head._last!=&entrys[2])return false;
	if( entrys[1]._prev || entrys[1]._next )return false;
	if(entrys[0]._prev != &entrys[0] )return false;
	if( entrys[0]._next != &entrys[2] )return false;
	if( entrys[2]._next )return false;
	if( entrys[2]._prev != &entrys[0] )return false;

	DBLIST_INSERT_AFTER( &head,&entrys[0], &entrys[1] );
			if(entrys[0]._prev!=&entrys[0])return false;
			if(entrys[0]._next!=&entrys[1] )return false;
			if(entrys[1]._prev!=&entrys[0])return false;
			if(entrys[1]._next!=&entrys[2] )return false;
			if(entrys[2]._prev!=&entrys[1])return false;
			if(entrys[2]._next!=0 )return false;

	return true;
}
*/