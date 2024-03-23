#!/bin/bash

:<<+
until condition
do
    command
done
+

a=0
until [ ! $a -lt 10 ]
do
   echo $a
   a=`expr $a + 1`
done

