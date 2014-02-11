CC=gcc
LIBS=-lpthread
CFLAGS=-I.
DEPS= $(wildcard *.h)
OBJ = $(patsubst %.c,%.o,$(wildcard *.c))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

undb: $(OBJ) $(DEPS)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY:clean
clean:
	rm -f *.o undb
