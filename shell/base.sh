#!/bin/bash

# $直接加数字，表示输入的字符串的第几段
echo "Program name is $0"

# 另一种注释方法
:<<!
变量赋值时等号两边无空格, 变量全大写
unset将删除对VAR的赋值
unset VAR
!
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
# $!: 后台运行的最后一个进程的ID号

# 使用readonly 修饰变量可以使变量只读
MYURL="https://www.baidu.com"
readonly MYURL
MYURL="https://www.google.com"

# 另一种注释方法
:<<EOF
使脚本有执行权限
chmod +x base.sh
执行脚本：
./base.sh
EOF
