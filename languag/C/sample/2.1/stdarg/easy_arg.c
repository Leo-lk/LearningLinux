#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "inc/jx_types.h"


void tLog(char *sInfo, ...)
{
    char *str   = NULL;
    /* 申请参数列表变量 */
    va_list sArgv;  
    /* 申明最后一个固定参数 */   
    va_start(sArgv, sInfo); 
    puts(sInfo);
    /* 依次读取固定参数 sInfo 后的 参数 */
    while(str = va_arg(sArgv,char*))
    {
        puts(str);
    }
    va_end(sArgv);
}

void tstdarg()
{
    char str[4+1] = {0};
    strcpy(str,"INFO");
    /* 多传入一个0作为while判断的终止，否则会有 “Segmentation fault (core dumped)” */
    /* 也可以第二个参数传入参数长度 */
    tLog(str,"argv1","argv2","argv3","argv4",0);
}

_i32 main(_i32 argc, _s argv[])
{
    tstdarg();
    return 0;
}
