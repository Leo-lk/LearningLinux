CC?=gcc
TARGET=main
OBJS=test1 test2 

$(TARGET):$(OBJS)
	@chmod +x test1
	@chmod +x test2

test1:t1.o
	@$(CC) -o $@ $^
test2:t2.o
	@$(CC) -o $@ $^

# $(TARGET):$(OBJS)
# 	$(CC) -o $@ $(OBJS)

%.o:%.c
	@$(CC) -c $^ -o $@

.PHONY:clean

clean:
	@rm $(OBJS) *.o