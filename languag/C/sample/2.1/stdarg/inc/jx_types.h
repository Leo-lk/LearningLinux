#ifndef _JX_TYPES_H
#define _JX_TYPES_H
#include <stdarg.h>

typedef signed char _i8;

typedef unsigned char _u8;

typedef signed int _i32;

typedef unsigned int _u32;

typedef _u32 _I;

typedef char _c;

typedef char * _s;

/*va_arg：从可变参存储空间中检索下一个类型为 type 的参数*/
#define _getva_T_( P, params, _T_)  do {P = ( _T_) va_arg( params, _T_); }while (0) 
/*从params中获取下一个_s类型的参数*/
#define _getva_S( s, params)  _getva_T_( s, params, _s)
#define _getva_I( i, params)  _getva_T_( i, params, _I)


#endif
