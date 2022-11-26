typedef struct{
    _I next;
    _I _prec;    
    _I data;
}_Inode;

#define _T_ _Inode
#define __allocname alloc_Inode
#define __freename free_Inode
#include "indexTemp.h"

#undef _T_
#define __allocname alloc_Inode
#define __freename free_Inode
#include "indexTemp.h"

