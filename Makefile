.POSIX:
.SUFFIXES:
.PHONY: all clean check install uninstall

CFLAGS = -ansi -pedantic -Wall
LDFLAGS =
CC = cc
RM = rm
CP = cp
SH = sh
MKDIR = mkdir
PREFIX = /usr/local

all: bin/langnd

clean:
	$(RM) -f bin/langnd obj/main.o obj/execute.o obj/parse.o obj/lex.o obj/utility.o

check: bin/langnd
	$(SH) test/run.sh bin/langnd

install: bin/langnd
	$(CP) bin/langnd $(DESTDIR)$(PREFIX)/bin/langnd

uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)/bin/langnd

bin/langnd: bin obj obj/main.o obj/execute.o obj/parse.o obj/lex.o obj/utility.o
	$(CC) $(LDFLAGS) -o bin/langnd obj/main.o obj/execute.o obj/parse.o obj/lex.o obj/utility.o

obj/main.o: src/main.c obj/execute.o obj/utility.o
	$(CC) $(CFLAGS) -c src/main.c -o obj/main.o

obj/execute.o: src/execute.h src/execute.c obj/parse.o obj/utility.o
	$(CC) $(CFLAGS) -c src/execute.c -o obj/execute.o

obj/parse.o: src/parse.h src/parse.c obj/lex.o obj/utility.o
	$(CC) $(CFLAGS) -c src/parse.c -o obj/parse.o

obj/lex.o: src/lex.h src/lex.c obj/utility.o
	$(CC) $(CFLAGS) -c src/lex.c -o obj/lex.o

obj/utility.o: src/utility.h src/utility.c
	$(CC) $(CFLAGS) -c src/utility.c -o obj/utility.o

bin:
	$(MKDIR) bin

obj:
	$(MKDIR) obj
