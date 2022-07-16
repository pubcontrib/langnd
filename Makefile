.POSIX:
.SUFFIXES:
.PHONY: all clean install uninstall

CFLAGS = -ansi -pedantic -Wall
LDFLAGS =
CC = cc
RM = rm
CP = cp
MKDIR = mkdir
PREFIX = prefix

all: bin/langnd

clean:
	$(RM) -f bin/langnd obj/main.o

install: bin/langnd
	$(CP) bin/langnd $(DESTDIR)$(PREFIX)/bin/langnd

uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)/bin/langnd

bin/langnd: bin obj obj/main.o
	$(CC) $(LDFLAGS) -o bin/langnd obj/main.o

obj/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o obj/main.o

bin:
	$(MKDIR) bin

obj:
	$(MKDIR) obj
