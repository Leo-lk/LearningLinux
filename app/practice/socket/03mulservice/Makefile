
CC=gcc
TARGET=main
OBJS=client service

$(TARGET):$(OBJS)
	@chmod +x client
	@chmod +x service

client:client.o
	@$(CC) -o $@ $^

service:service.o
	@$(CC) -o $@ $^

# $(TARGET):$(OBJS)
# 	$(CC) -o $@ $(OBJS)

%.o:%.c
	@$(CC) -c $^ -o $@

.PHONY:clean

clean:
	@rm $(OBJS) *.o
