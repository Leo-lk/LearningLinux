#!/usr/bin/env python3

import os

# os.chdir(path)	改变当前工作目录
# os.getcwd()	    返回当前python的工作目录
# os.curdir         返回 . 表示python文件目录，os.path.abspath(os.curdir)就和os.getcwd返回一样了
# os.listdir(path)  返回path指定的文件夹包含的内容列表。
# os.path.splitext  分离文件名与扩展名；默认返回(fname,fextension)元组
# os.sep            根据系统适配路径分隔符'\'或'/'
def search_file(start_dir, target) :
    os.chdir(start_dir)
    
    for each_file in os.listdir(os.curdir) :
        ext = os.path.splitext(each_file)[1]
        if ext in target :
            vedio_list.append(os.getcwd() + os.sep + each_file + os.linesep) 
        if os.path.isdir(each_file) :
            search_file(each_file, target) # 递归调用
            os.chdir(os.pardir) # 递归调用后切记返回上一层目录

start_dir = input('请输入待查找的初始目录：')
program_dir = os.getcwd()

target = ['.mp4', '.avi', '.rmvb']
vedio_list = []

search_file(start_dir, target)

f = open(program_dir + os.sep + 'vedioList.txt', 'w')
f.writelines(vedio_list)
f.close()


