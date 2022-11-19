obj = lktmp
objects = 2.o

$(obj):$(objects)
	gcc -o $@  $^
%.o:%.c 
	gcc -c $^ -o $@

.PHONY:clean
clean:
	rm *.o $(obj) &
