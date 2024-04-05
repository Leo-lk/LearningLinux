#!/bin/bash

# For init compile env, source this file with input before compile, 

input="${1}"

if [ ${input} == "ask" ]; then
    echo "Set env for 100ask"
    export ARCH=arm
    export CROSS_COMPILE=arm-linux-gnueabihf-
    export PATH=$PATH:/home/eden/tools/100ask_imx6ull-qemu/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin
    export KDIR=/home/eden/tools/100ask_imx6ull-qemu/linux-4.9.88
    export CC=arm-linux-gnueabihf-gcc
elif [ ${input} == "atk" ]; then
    echo "Set env for atk"
    export ARCH=arm
    export CROSS_COMPILE=arm-linux-gnueabihf-
    export PATH=$PATH:/opt/atk-dlrv1126-toolchain/bin
    export KDIR=/home/eden/atk/kernel
    export CC=arm-linux-gnueabihf-gcc
elif [ ${input} == "this" ]; then
    export KDIR=/lib/modules/$(shell uname -r)/build
    export CC=gcc
else
    echo "ask  : 100ask"
    echo "atk  : atk"
    echo "this : ubuntu"
fi
