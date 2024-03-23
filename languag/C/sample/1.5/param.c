/*
1.5
*/
#include <stdio.h>
#include <string.h>
#include "param.h"

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

int chk_param(int argc,  char *argv[]){
    int re = -1;   
    int i;  

    _error(  argc < 2,  _chk_param_END," param less !  only %d\n",  argc);  
    _error(  argv[1][0] != '-',  _chk_param_END,  "the %s param error ,  need '-'\n",  argv[1]);  

    for (  i = 0 ;   i < _MAX_MODE ;  i++)  {
        if (  strcmp(  &argv[1][1],  MODESTR[i])   == 0)
        {
            chkParam[i](  argc-2,  argv+2);   
            re = 0;  
            goto _chk_param_END;  
        }
    }
    _error(  i >= _MAX_MODE,  _chk_param_END,  " no found this Mode %s\n",  argv[1]);  
_chk_param_END:  
    return re;  
}      
