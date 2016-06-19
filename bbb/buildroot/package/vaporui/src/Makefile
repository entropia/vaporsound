OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: vaporui

vaporui: $(OBJS)
	$(CC) -o vaporui $(LDFLAGS) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I. -Wall -c -o $@ $<

clean:
	rm -f *.o vaporui
