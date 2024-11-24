CFLAGS = -g -O2 -Wall
CC = gcc
OBJS = build/grid.o build/main.o build/draw.o

cudoku: $(OBJS)
	$(CC) -o cudoku $(OBJS) -lncurses

build/%.o: src/%.c
	$(CC) -c -o $@ $^ $(CFLAGS)
