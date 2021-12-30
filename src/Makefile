CFLAGS = -O2 -Wall

netrate: netrate.c netrate.h list.h
	$(CC) $(CFLAGS) -o netrate netrate.c
	strip netrate

static: netrate.c netrate.h list.h
	$(CC) $(CFLAGS) -o netrate -static netrate.c
	strip netrate

debug:
	$(CC) -O0 -g -lefence -o netrate netrate.c

clean:
	rm -f netrate
