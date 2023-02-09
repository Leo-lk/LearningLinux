
CC=gcc
TARGET=main
OBJS=recv send

$(TARGET):$(OBJS)
	@chmod +x recv
	@chmod +x send

recv:recv.o
	@$(CC) -o $@ $^

send:send.o
	@$(CC) -o $@ $^

# $(TARGET):$(OBJS)
# 	$(CC) -o $@ $(OBJS)

%.o:%.c
	@$(CC) -c $^ -o $@

.PHONY:clean

clean:
	@rm $(OBJS) *.o
