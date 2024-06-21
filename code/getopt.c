#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char * argv[])
{
    
    int ch;
 
    printf("初始值：optind = %d，opterr = %d\n", optind, opterr);
 
    while ((ch = getopt(argc, argv, "vha:b:c::")) != -1)
    {
        printf("开始解析参数：optind = %d\n", optind);
        switch (ch) 
        {
                case 'v':
                        printf("捕获到选项: -v\n");
                        printf("系统版本v1.1.0\n");   
                        break;
                case 'h':
                        printf("捕获到选项: -h\n"); 
                        printf("用法示例： -a 1 -b 2 -c [\"hello\"]\n");
                        break;
                case 'a':
                        printf("捕获到选项: -a\n");
                        printf("该选项参数为：%s\n\n", optarg);
                        printf("该选项参数为：%d\n\n", atoi((const char*)optarg));
                        break;
                case 'b':
                        printf("捕获到选项: -b\n");
                        printf("该选项参数为：%s\n\n", optarg);
                        break;
                case 'c':
                        printf("捕获到选项: -c\n");
                        printf("该选项参数为：%s\n\n", optarg);
                        break;
        }
    }
    
    /* Print remaining arguments. */
    for (; optind < argc; optind++)
        printf("%s\n", argv[optind]);
    
    return 0;
}