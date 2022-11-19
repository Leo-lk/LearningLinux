
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

/*如果定义为全局数据，就可以被其他C文件调用到，实现跨C文件的跳转*/
static jmp_buf jmp; 

float a( float i, float j) {
    if ( j == 0) {
        /*如果出现错误j=0，直接跳转回main中*/
        longjmp( jmp, 2); 
    }
    return i / j; 
}

float b( float x1, float x2) {
        return  a( x1+x2, x1-x2); 
}

int main( int argc, char *argv[]) {
    float x1, x2; 
    assert( argc > 2); 
    x1 = atof( argv[1]); 
    x2 = atof( argv[2]);
    if ( setjmp( jmp)  == 0) {
        printf( "( %f + %f) / ( %f - %f)  = %f\n", x1, x2, x1, x2, b( x1, x2)); 
    }else{
        /*longjmp被调用之后的分支*/
        printf( "( %f - %f)  error! \n", x1, x2); 
    }
    return 0; 
}
