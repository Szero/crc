IFROOT=$(shell id -u)

BIN=crc32
DESTDIR=/usr/local/bin

VERSION = 0.1

CC=gcc

ifdef DEBUG
CFLAGS=-c -Wall -pedantic -std=c11 -g -DVERSION=\"$(VERSION)\"
else
CFLAGS=-c -Wall -pedantic -std=c11 -O2 -DVERSION=\"$(VERSION)\"
endif

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:%.c=%.o)

all: $(OBJECTS) $(BIN)

$(OBJECTS): %.o : %.c
	@echo "Creating objects ..."
	$(CC) $(CFLAGS) $< -o $@

$(BIN): $(OBJECTS)
	@echo "Creating binary ..."
	$(CC) $(OBJECTS) -o $@

install: all
ifeq ($(IFROOT),0)
	@echo "Installing into ${DESTDIR}"
	mkdir -p ${DESTDIR}
	install -m 755 -t ${DESTDIR} ${BIN}
else
	@echo "You have to be root in order to install the program!"
endif

uninstall:
ifeq ($(IFROOT),0)
	@echo "Removing executable from ${DESTDIR}"
	rm -f ${DESTDIR}/${BIN}
else
	@echo "You have to be root in order to uninstall the program!"
endif

dist: clean
	mkdir -p crc-$(VERSION)
	cp -fR makefile .ycm_extra_conf.py *.h $(SOURCES) crc-$(VERSION)
	tar -cf - crc-$(VERSION) | gzip > crc-$(VERSION).tar.gz
	rm -rf crc-$(VERSION)

clean:
	@echo "Cleaning objects and binary ..."
	rm -f ${OBJECTS} ${BIN} crc-$(VERSION).tar.gz

.PHONY: all install uninstall clean dist
