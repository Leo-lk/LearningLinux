#ifndef _JX_DELOG_H
#define _JX_DELOG_H

#include "jx_types.h"

#define _POS_INFOBITS ( _I)1
#define _NPOS_INFOBITS ( ~_POS_INFOBITS)
enum {
    _INIT_INFO_DELOG = 0, 
    _MAX_INFO_DELOG, 
}; 

void _jx_delog( _I type, ...);


#define _pos_delog( type, ...)      _jx_delog(( type) |_POS_INFOBITS, __FILE, __LINE__, __func__, __VA_ARGS__) 
#define _Npos_delog( type, ...)     _jx_delog(( type) &_NPOS_INFOBITS, __VA_ARGS__) 
#define init_delog()                do{_Npos_delog( _INIT_INFO_DELOG,); }while (0) 

#endif /* _JX_DELOG_H */