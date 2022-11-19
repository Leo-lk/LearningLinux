obj = tmp
objects = easy_arg.o
dir_obj=obj
dir_exe=bin
dir_src=src

$(obj):$(objects)
	gcc -o ${dir_exe}/$@  ${dir_obj}/$^
%.o:%.c
	gcc -c $^ -o ${dir_obj}/$@
	

.PHONY:clean
clean:
	rm ${dir_obj}/*.o ${dir_exe}/$(obj) &
