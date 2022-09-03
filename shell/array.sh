#!/bin/bash

# bash支持一维数组（不支持多维数组）
array_name=(value0 value1 value2 value3)

array_name=(
value0
value1
value2
value3
)

valuen=${array_name[n]}

# 使用 @ 符号可以获取数组中的所有元素
echo ${array_name[@]}
# 取得数组元素的个数
length=${#array_name[@]}
# 或者
length=${#array_name[*]}
# 取得数组单个元素的长度
lengthn=${#array_name[n]}

