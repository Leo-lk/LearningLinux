#!/bin/bash

VAR1=5
VAR2=4

# expr：算数运算命令
ADD=`expr $VAR1 + $VAR2`
MUL=`expr $VAR1 \* $VAR2`
echo "ADD = $ADD, MUL = $MUL"
# 或者
ADD=$[ $VAR1 + $VAR2 ]
MUL=$[ $VAR1 * $VAR2]
echo "ADD = $ADD, MUL = $MUL"
