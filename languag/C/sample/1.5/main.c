/*
1.5
*/
#include "param.h"


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

int main(int argc,  char *argv[])
{
    chk_param(argc, argv);

_main_END:
    return 0;
}

