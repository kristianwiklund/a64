
%.co: %.c
	$(CC) $(CFLAGS) -c -g -fprofile-arcs -ftest-coverage -o $@ $<
 
CFLAGS = -O2

all: a64 l64 a64cov

yaccer.c: yaccer.y
	bison -v -d -o yaccer.c yaccer.y

yaccer.h: yaccer.c

lexer.c: lexer.l yaccer.h
	flex -i -olexer.c lexer.l

a64: l64 ymain.o lexer.o yaccer.o mnemonics.o emit.o
	$(CC) -o a64 ymain.o yaccer.o lexer.o mnemonics.o emit.o

a64cov: l64 ymain.co lexer.co yaccer.co mnemonics.co emit.co
	$(CC) -o a64cov ymain.co yaccer.co lexer.co mnemonics.co emit.co

ci: clean
	ci -q -mx *.c *.h *.l *.y  docs.txt
	ci -q -mx requirements.txt
	rcs -U RCS/*
	co -q RCS/*

clean:
	rm -f #*#
	rm -f a64??????
	rm -f *.bb
	rm -f *.bbg
	rm -f *.gcov
	rm -f *.o
	rm -f *.co
	rm -f *.da
	rm -f yaccer.h
	rm -f lexer.c
	rm -f yaccer.c
	rm -f #*#
	rm -f *~
	rm -f a64
	rm -f yaccer.output
	rm -f *.exe
	rm -f l64
	rm -f lnk.c
	rm -f a64cov

test:   a64
	./a64 tests/test.asm
	./a64 tests/border.asm

install: a64
	cp a64 /usr/local/bin
	mkdir -p /usr/local/lib/a64
	cp -R include /usr/local/lib/a64




