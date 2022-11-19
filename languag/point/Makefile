obj = lktmp
objects = lktmp.o

$(obj):$(objects)
	gcc -o $@  $^
%.o:%.c 
	gcc -c $^ -o $@

.PHONY:clean
clean:
	rm % $(objects) $(obj)
