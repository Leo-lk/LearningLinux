#!/usr/bin/env python3
import os

path=input('请输入文件路径(结尾加上/):')

#获取该目录下所有文件，存入列表中
fileList=os.listdir(path)

n=0
for i in fileList:
    
    #设置旧文件名（就是路径+文件名）
    oldname=path+ os.sep + fileList[n]   # os.sep添加路径分隔符
    
    #设置新文件名
    newname=path + os.sep +'a'+str(n+1)+'.JPG'
    
    os.rename(oldname,newname)   #用os模块中的rename方法对文件改名
    print(oldname,'======>',newname)
    
    n+=1

# os.replace(old, new) 将文件重命名。用new的名字替换给old，并删除new，（内容不变，只替换名字）


