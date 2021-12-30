CFLAGS ?= -O2 -Wall
INSTALL ?= install

.PHONY: install clean

netrate: netrate.c netrate.h list.h

install: netrate
	$(INSTALL) -m 755 -D netrate $(DESTDIR)/usr/bin/netrate

clean:
	rm -f netrate
