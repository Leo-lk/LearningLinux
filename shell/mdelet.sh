#!/bin/bash

echo "Program name is $0"
if test $# -eq 0
then
    echo "Please input file"
else
    rm $1
    echo "Delete $1"
fi
