#!/bin/bash

# $直接加数字，表示输入的字符串的第几段
echo "Program name is $0"

# 变量赋值时等号两边无空格, 变量全大写
# unset将删除对VAR的赋值
# unset VAR
VAR=$0
echo "Program name is ${VAR}"
# $#：包含命令行参数的个数
echo "There are totally $# parameters passed to this program"
# $?: 包含前一个命令的退出状态，正常退出返回0，反之为非0值
echo "The last is $?"
# $*: 包含所有命令行参数：“$1,$2,…,$9”。返回单个字符串
echo "The parameters are $*"
# $@: 包含所有命令行参数：“$1,$2,…,$9”。返回多个字符串，可以用for遍历
echo "The parameters are $@"
# $$：包含正在执行进程的ID号
