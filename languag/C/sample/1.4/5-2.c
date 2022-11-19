/*
1.4.5
*/
#include <stdio.h>
#include <string.h>

typedef int (*pCHKPARAMFUNC)(int argc, char *argv[]);

enum{
    #define ENUM_INFO
    #include "paramDef.h"
    _MAX_MODE
};
/*定义字符串数组*/
const char * const MODESTR[_MAX_MODE] = {
#define MODESTR_INFO
#include "paramDef.h"
};
/*函数声明*/
#define FUNCDECLARE_INFO
#include "paramDef.h"
/*定义函数指针数组*/
static const pCHKPARAMFUNC chkParam[_MAX_MODE] = {
#define FUNCNAME_INFO
#include "paramDef.h"
};


int g_chk_paramA(int argc, char *argv[])
{
    printf("is mode A! first param is %s\n ",argv[0]);
    return 0;
}
int g_chk_paramB(int argc, char *argv[])
{
    printf("is mode B! first param is %s\n ",argv[0]);
    return 0;
}
int g_chk_paramC(int argc, char *argv[])
{
    printf("is mode C! first param is %s\n ",argv[0]);
    return 0;
}


#define __info(fmt,...)     do{\
    printf("[%s]<%s>(%d):",__FILE__, __func__, __LINE__); \
    printf( fmt, __VA_ARGS__); \
    }while(0)

#define mkstr(symbol) #symbol

#define _error(exp, escape_lable, fmt, ...)do{\
        if (exp)\
        {\
            __info(" *** error occured *** | %s",mkstr(exp));\
            printf(fmt, __VA_ARGS__);\
            goto escape_lable;\
        }\
    }\
    while (0)

#define _pos()  __info("%s","\n")


int main(int argc, char *argv[])
{
    int i;

    _error(argc < 2, _main_END, "param less ! only %d\n ",argc);
    /*检测第一个参数是否以“-”开头*/
    _error(argv[1][0] != '-',_main_END,"the %s param error ,need '-'\n ",argv[1]);

    for (i = 0; i < _MAX_MODE; i++)
    {
        /*循环检测第一个参数从第二字符开始的字符串是否和MODESTR中指向的某个字符串内容相同*/
        if (strcmp(&argv[1][1],MODESTR[i])== 0)
        {
            chkParam[i](argc - 2, argv + 2);

            goto _main_END;
        }
    }
    _error(i >= _MAX_MODE, _main_END, " no found this Mode %s\n", argv[1]);

_main_END:
    return 0;
}
