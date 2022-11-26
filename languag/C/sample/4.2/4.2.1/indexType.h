/* 定义默认类型（一次性引用） */
#ifndef __INDEXTYPE_H__
#define __INDEXTYPE_H__
typedef struct{
    _I _next;
    _I _prec;
}__INDEX;
#endif

/* 定义_T_，默认为__INDEX */
#ifndef _T_
#define _T_ __INDEX
#endif