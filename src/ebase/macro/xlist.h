#pragma once

#include <assert.h>
/*
head:_first,_last
entry:_prev,_next
��һ��Ԫ��_prev�����Լ�
*/
#define list_assert(cond) assert(cond)

#define XLIST_NULL_PTR 0

#ifndef _XLIST_FIRST_SETTER
#define _XLIST_FIRST_SETTER( host,entry ) ((host)->_first=entry)
#define _XLIST_FIRST_GETTER( host ) ((host)->_first)

#define _XLIST_LAST_SETTER( host,entry ) ((host)->_last=entry)
#define _XLIST_LAST_GETTER( host ) ((host)->_last)

#define _XLIST_PREV_SETTER( entry,entrynew ) ((entry)->_prev=entrynew)
#define _XLIST_PREV_GETTER( entry ) ((entry)->_prev)

#define _XLIST_NEXT_SETTER( entry,entrynew ) ((entry)->_next=entrynew)
#define _XLIST_NEXT_GETTER( entry ) ((entry)->_next )
#endif

#define XLIST_ENTRY_INIT( _entry )\
	_XLIST_PREV_SETTER(_entry,XLIST_NULL_PTR);\
	_XLIST_NEXT_SETTER(_entry,XLIST_NULL_PTR)

#define XLIST_HOST_INIT( _host )\
	_XLIST_FIRST_SETTER(_host,XLIST_NULL_PTR);\
	_XLIST_LAST_SETTER(_host,XLIST_NULL_PTR) 


#define _XLIST_SET_FIRST(_host,_entry)\
{\
	if( XLIST_NULL_PTR == _XLIST_LAST_GETTER(_host) ) _XLIST_LAST_SETTER(_host, _entry);\
	if( XLIST_NULL_PTR == _entry )_XLIST_LAST_SETTER(_host,XLIST_NULL_PTR);\
	_XLIST_FIRST_SETTER(_host, _entry);\
}

#define _XLIST_SET_LAST(_host,_entry)\
{\
	if( XLIST_NULL_PTR == _XLIST_FIRST_GETTER(_host) ) _XLIST_FIRST_SETTER(_host,_entry);\
	if( XLIST_NULL_PTR == _entry )_XLIST_FIRST_SETTER(_host,XLIST_NULL_PTR);\
	_XLIST_LAST_SETTER(_host,_entry);\
}
#define _XLIST_REMOVE( _entry )\
{\
	if( _XLIST_PREV_GETTER(_entry) ) _XLIST_NEXT_SETTER(_XLIST_PREV_GETTER(_entry), _XLIST_NEXT_GETTER(_entry));\
	if( _XLIST_NEXT_GETTER(_entry) ) _XLIST_PREV_SETTER(_XLIST_NEXT_GETTER(_entry),_XLIST_PREV_GETTER(_entry));\
}

//��_entry_next�ŵ�_entry����
#define XLIST_INSERT_AFTER( _entry ,_entry_next )	\
{\
	_XLIST_PREV_SETTER(_entry_next,_entry);\
	_XLIST_NEXT_SETTER(_entry_next,(XLIST_NULL_PTR==_entry)?XLIST_NULL_PTR:_XLIST_NEXT_GETTER(_entry) );\
	if( _entry&&_XLIST_NEXT_GETTER(_entry) )_XLIST_PREV_SETTER(_XLIST_NEXT_GETTER(_entry),_entry_next);\
	if(_entry)_XLIST_NEXT_SETTER(_entry,_entry_next);\
}

//��_entry_prev�ŵ�_entryǰ��
#define XLIST_INSERT_BEFORE( _entry ,_entry_prev )	\
{\
	_XLIST_PREV_SETTER(_entry_prev, (XLIST_NULL_PTR==_entry)?XLIST_NULL_PTR:_XLIST_PREV_GETTER(_entry) );\
	_XLIST_NEXT_SETTER(_entry_prev, _entry);\
	if( _entry && _XLIST_PREV_GETTER(_entry) ) _XLIST_NEXT_SETTER(_XLIST_PREV_GETTER(_entry), _entry_prev);\
	if(_entry)_XLIST_PREV_SETTER(_entry, _entry_prev);\
}


#define XLIST_PUSH_BACK( _host,_entry )\
	{\
		XLIST_INSERT_AFTER( _XLIST_LAST_GETTER(_host),_entry );\
		_XLIST_SET_LAST(_host,_entry) ;\
	}

#define XLIST_PUSH_FRONT( _host,_entry )\
	{\
	XLIST_INSERT_BEFORE(_XLIST_FIRST_GETTER(_host),_entry);\
	_XLIST_SET_FIRST(_host,_entry) ;\
	}


//_entry_new���뵽_entry����,���_entryΪXLIST_NULL_PTR,���뵽��ǰ��
#define XLIST_INSERT( _host,_entry,_entry_new )\
{\
	if( XLIST_NULL_PTR == _entry ) \
	{\
		XLIST_PUSH_FRONT(_host,_entry_new);\
	}\
	else if( _entry == _XLIST_LAST_GETTER(_host ) ) \
	{\
		XLIST_PUSH_BACK( _host,_entry_new);\
	}\
	else \
	{\
		XLIST_INSERT_AFTER( _entry,_entry_new);\
	}\
}



//ɾ����ǰ���Ԫ��
#define XLIST_POP_FRONT( _host )\
	{\
	_XLIST_REMOVE( _XLIST_FIRST_GETTER(_host) );\
	_XLIST_SET_FIRST(_host,_XLIST_NEXT_GETTER( _XLIST_FIRST_GETTER(_host) ));\
	}

#define XLIST_POP_BACK( _host )\
	{\
	_XLIST_REMOVE( _XLIST_LAST_GETTER(_host) );\
	_XLIST_SET_LAST(_host,_XLIST_PREV_GETTER( _XLIST_LAST_GETTER(_host) ));\
	}

#define XLIST_ERASE( _host,_entry )\
	if( _entry == _XLIST_FIRST_GETTER(_host ) )\
	{\
		XLIST_POP_FRONT(_host);\
	}else if( _entry == _XLIST_LAST_GETTER(_host ) )\
	{\
		XLIST_POP_BACK(_host);\
	}else\
	{\
		_XLIST_REMOVE(_entry);\
	}
