#!/bin/bash

:<<-
# echo 显示指定的环境变量
# export 设置新的环境变量
# env 显示所有的环境变量
-
echo $PATH
export NEW_P="test"
env
