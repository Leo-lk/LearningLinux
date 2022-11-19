obj = tmp
objects = main.o param.o

$(obj):$(objects)
	gcc -o $@  $^
%.o:%.c
	gcc -c $^ -o $@

.PHONY:clean
clean:
	rm *.o $(obj) &
