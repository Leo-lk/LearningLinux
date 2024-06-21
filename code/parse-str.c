#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void parse_command_line(const char *input, int *argc, char ***argv) {
    // 初始化参数计数和参数数组
    *argc = 0;
    *argv = NULL;
    int capacity = 10;
    *argv = malloc(capacity * sizeof(char *));
    if (*argv == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    const char *p = input;
    while (*p != '\0') {
        // 跳过空白字符
        while (isspace((unsigned char)*p)) {
            p++;
        }

        if (*p != '\0') {
            if (*argc >= capacity) {
                capacity *= 2;
                *argv = realloc(*argv, capacity * sizeof(char *));
                if (*argv == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    exit(EXIT_FAILURE);
                }
            }

            // 找到下一个参数的起始位置
            const char *start = p;
            while (*p != '\0' && !isspace((unsigned char)*p)) {
                p++;
            }

            // 复制参数字符串
            int len = p - start;
            (*argv)[*argc] = malloc(len + 1);
            if ((*argv)[*argc] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            strncpy((*argv)[*argc], start, len);
            (*argv)[*argc][len] = '\0';
            (*argc)++;
        }
    }
}

void free_argv(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
}

int main(int argc1, char **argv1) {
    const char *input = "program -arg1 value1 -arg2 value2";
    int argc;
    char **argv;

    parse_command_line(input, &argc, &argv);

    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    // 释放分配的内存
    free_argv(argc, argv);

    return 0;
}
