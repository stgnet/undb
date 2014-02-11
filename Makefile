CC=gcc
LIBS=-lpthread
CFLAGS=-I.
DEPS=
OBJ = $(patsubst %.c,%.o,$(wildcard *.c))

%.o: .c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

undb: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY:clean
clean:
	rm -f *.o undb
