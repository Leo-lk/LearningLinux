CC?=gcc
TARGET=main
OBJS=test 

$(TARGET):$(OBJS)
	@chmod +x test

test:t1.o
	@$(CC) -o $@ $^

# $(TARGET):$(OBJS)
# 	$(CC) -o $@ $(OBJS)

%.o:%.c
	@$(CC) -c $^ -o $@

.PHONY:clean

clean:
	@rm $(OBJS) *.o