.PHONY:clean
#CC = gcc
RM = rm
OBJ = simple
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

$(OBJ):$(OBJS)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) -c $^ -o $@

clean:
	@$(RM) $(OBJS) $(OBJ)

#SOURCE = $(wildcard *.s)

#all:
#	@echo $(SOURCE)

