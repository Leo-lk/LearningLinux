#include <stdio.h>

int func(int n)
{
    int sum ;
    for(int i = 0; i<=n ;i++ )
        sum += i;
    return sum;
}


int main (int argc, char **argv)
{
    int i = 0;
    long res = 0;
    for (; i < 100; i++)
    {
        res += i;
    }
    printf("res = %ld\n", res);
    printf("n = %d\n", func(250));
}


