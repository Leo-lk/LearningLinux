.PHONY:clean
CC = gcc
RM = rm
OBJ = simple
SOURCE = $(wildcard *.c)

OBJS = $(patsubst %.c,%.o,$(SOURCE))

$(OBJ):$(OBJS)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) -c $^ -o $@

clean:
	@$(RM) $(OBJS) $(OBJ)


