all: clean $(EXE)

include Makefile.rules

$(EXE): a.c clean
		$(CC) -isysroot "$(SDKROOT)" -arch i386 -g -c $(SRCDIR)/a.c
		ar -q liba-i386.a a.o
		ranlib liba-i386.a
		$(CC) -isysroot "$(SDKROOT)" -arch x86_64 -g -c $(SRCDIR)/a.c
		ar -q liba-x86_64.a a.o
		ranlib liba-x86_64.a
		lipo -create -output liba.a liba-i386.a liba-x86_64.a
		$(CC) -isysroot "$(SDKROOT)" -g -c $(SRCDIR)/main.c
		$(CC) -isysroot "$(SDKROOT)" -o a.out main.o -L. -la

clean::
		rm -rf a.o a.out liba-i386.a liba-x86_64.a liba.a $(wildcard *un~ .*un~ main.o *.pyc)

