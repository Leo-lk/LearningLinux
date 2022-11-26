/* 调用默认类型的定义，并不对_T_做判断 */
#include "indexType.h"
#ifdef __allocname
_T_ *__allocname( _I num) {
    _T_ *Pre = 0; 
    _error( num == 0, __func_END, "num zero! "); 
    Pre = ( _T_ *) malloc( sizeof( _T_)  * num); 
__func_END: 
    return Pre; 
}
#endif
#undef __allocname
#ifdef __freename
void __freename( _T_ ** ppt) {
    if ( *ppt) {
        free( *ppt);  
        *ppt = 0; 
    }
    return ; 
}
#endif
#undef __freename
/* 不对_T_做undef处理 */