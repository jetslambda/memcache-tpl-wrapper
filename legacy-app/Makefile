#
# Legacy Application's Simple Build Environment
#

CC=gcc
CP=cp
CFLAGS=-Wall -g -I.
VERSION=1.0

legacy: legacy.o
	$(CC) $(CFLAGS) -o legacy legacy.o


package: legacy
	mkdir -p legacy-$(VERSION)/usr/local/bin
	mkdir -p legacy-$(VERSION)/DEBIAN
	echo "Package: legacy-$(VERSION)\n\
	Version: $(VERSION)\n\
	Section: web\n\
	Priority: optional\n\
	Architecture: all\n\
	Essential: no\n\
	Installed-size: 1024\n\
	Maintainer: Brian McManus <bmcmanus@gmail.com>\n\
	Description: ACME Inc Legacy Server (C) 1999-2012" > legacy-$(VERSION)/DEBIAN/control
	$(CP) legacy legacy-$(VERSION)/usr/local/bin/legacy-$(VERSION)
	dpkg --build legacy-$(VERSION)
	
clean:
	rm -rf *.o legacy legacy-$(VERSION).deb legacy-$(VERSION)
