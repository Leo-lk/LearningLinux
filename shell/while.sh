#!/bin/bash

int=1
while(( $int<=5 ))
do
    echo $int
    # 用于执行一个或多个表达式, 变量计算中不需要加上 $ 来表示变量
    let "int++"
done


echo '按下 <CTRL-D> 退出'
echo -n '输入你最喜欢的网站名: '
while read FILM
do
    echo "是的！$FILM 是一个好网站"
done

:<<-
无限循环
while true
do
    command
done
或者
for (( ; ; ))
-
