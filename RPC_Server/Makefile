CC = gcc
CFLAGS =  -Wall -Iinclude
LDFLAGS = -lwiringPi
TARGET = rpc_server
SRCS = src/main.c src/motor.c src/uart.c src/stub.c src/lcd.c src/can.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)