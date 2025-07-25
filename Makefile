CFLAGS = -g -O2 -Wall
CC = gcc
OBJS = build/grid.o build/main.o build/draw.o build/solver.o

cudoku: $(OBJS)
	$(CC) -o cudoku $(OBJS) -lncurses

build/%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f build/*
	rm -f cudoku
