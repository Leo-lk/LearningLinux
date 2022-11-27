
CC=gcc
TARGET=main
OBJS=con pro

$(TARGET):$(OBJS)
	@chmod +x con
	@chmod +x pro

con:consume.o
	@$(CC) -o $@ $^

pro:product.o
	@$(CC) -o $@ $^

# $(TARGET):$(OBJS)
# 	$(CC) -o $@ $(OBJS)

%.o:%.c
	@$(CC) -c $^ -o $@

.PHONY:clean

clean:
	@rm $(OBJS) %.o
