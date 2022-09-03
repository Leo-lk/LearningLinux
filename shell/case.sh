#!/bin/bash


if [ $# -eq 0 ]
then
    echo "No argument is declared"
    exit
fi

case $1 in
    file1)
        echo "User selects file1"
        ;;
    file2)
        echo "User selects file2"
        ;;
    *)
        echo "You must select either file1 or file2!"
        ;;
esac



