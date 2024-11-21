CFLAGS = -g -O2 -Wall
CC = gcc
OBJS = build/main.o

cudoku: $(OBJS)
	$(CC) -o cudoku $(OBJS) -lncurses

build/%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)
