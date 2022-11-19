/*
1.4.5
*/
#include <stdio.h>
#include <string.h>

enum
{
    _MODEA,
    _MODEB,
    _MAX_MODE
};

const char *const MODESTR[_MAX_MODE] = {"A", "B"};

typedef int (*pCHKPARAMFUNC)(int argc, char *argv[]);

int g_chk_paramA(int, char *[]);

int g_chk_paramB(int, char *[]);

static const pCHKPARAMFUNC chkParam[_MAX_MODE] =
    {
        g_chk_paramA,
        g_chk_paramB};

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
    _error(argv[1][0] != '-',_main_END,"the %s param error ,need '-'\n ",argv[1]);

    for (i = 0; i < _MAX_MODE; i++)
    {
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