OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: vaporctl

vaporctl: $(OBJS)
	$(CC) -o vaporctl $(LDFLAGS) $(OBJS) -lasound -lmicrohttpd -lm

%.o: %.c
	$(CC) $(CFLAGS) -I. -Wall -c -o $@ $<

clean:
	rm -f *.o vaporctl
