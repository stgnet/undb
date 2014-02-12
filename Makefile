CC=gcc
LIBS=-lpthread
CFLAGS=-g -I. -D_GNU_SOURCE
DEPS= $(wildcard *.h) $(wildcard html/*.h)
OBJ = $(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.c,%.o,$(wildcard html/*.c))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

undb: $(OBJ) $(DEPS)
	gcc -o $@ $(OBJ) $(CFLAGS) $(LIBS)

.PHONY:clean
clean:
	rm -f *.o html/*.o undb 
