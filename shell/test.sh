#!/bin/bash

read VAR1
read VAR2

# 字符串测试
[ "$VAR1" = "$VAR2" ] # 注意空格
echo $?
[ "$VAR1" != "$VAR2" ]
echo $?
[ -z "$VAR1" ] # 字符串长度是否为0
echo $?
[ -n "$VAR1" ] # 字符串长度是否大于0
echo $?

# 满足条件，返回ture
if test -z $VAR2
then
    echo "$VAR2 len = 0" 
else
    echo "$VAR2 len != 0" 
fi

# 整数测试
if test $VAR1 = $VAR2
then
    echo "$VAR1 = $VAR2" 
else
    echo "$VAR1 != $VAR2" 
fi

# -eq # ==
# -ne # !=
# -gt # >
# -ge # >=
# -lt # <
# -le # <=
