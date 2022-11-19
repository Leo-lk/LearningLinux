#!/bin/bash
targetFiles=$(ls $1)   # 或者`ls`

for file in ${targetFiles}
do
    if [ $file == cn2en.sh ];
    then
        continue;
    fi
    sed -i 's/；/;/g' ${file}
    sed -i 's/，/,/g' ${file}
    sed -i 's/（/(/g' ${file}
    sed -i 's/）/)/g' ${file}
    sed -i 's/ / /g'  ${file}
    sed -i 's/！/!/g'  ${file}
    sed -i 's/：/:/g'  ${file}
done

echo "finish !"

