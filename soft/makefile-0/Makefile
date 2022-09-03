objects = main.o kbe.o commond.o insert.o

hello:$(objects)

kbe.o commond.o:commond.h
main.o kbe.o commond.o insert.o:buffer.h

.PHONY:clean
clean:
	rm hello $(objects)


