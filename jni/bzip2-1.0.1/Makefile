
SHELL=/bin/sh
BIGFILES=-D_FILE_OFFSET_BITS=64
MYCFLAGS=-Wall -Winline -O2 -fomit-frame-pointer -fno-strength-reduce $(BIGFILES) $(CFLAGS)

OBJS= blocksort.o  \
      huffman.o    \
      crctable.o   \
      randtable.o  \
      compress.o   \
      decompress.o \
      bzlib.o

all: libbz2.a bzip2 bzip2recover

bzip2: libbz2.a bzip2.o
	$(CC) $(MYCFLAGS) -o bzip2 bzip2.o -L. -lbz2 $(LDFLAGS)

bzip2recover: bzip2recover.o
	$(CC) $(MYCFLAGS) -o bzip2recover bzip2recover.o $(LDFLAGS)

libbz2.a: $(OBJS)
	rm -f libbz2.a
	$(AR) cq libbz2.a $(OBJS)
	$(RANLIB) libbz2.a ; \

install: bzip2 bzip2recover
	echo $(PREFIX)
	cp -f bzip2 $(PREFIX)/bin/bzip2
	cp -f bzip2 $(PREFIX)/bin/bunzip2
	cp -f bzip2 $(PREFIX)/bin/bzcat
	cp -f bzip2recover $(PREFIX)/bin/bzip2recover
	chmod a+x $(PREFIX)/bin/bzip2
	chmod a+x $(PREFIX)/bin/bunzip2
	chmod a+x $(PREFIX)/bin/bzcat
	chmod a+x $(PREFIX)/bin/bzip2recover
	cp -f bzip2.1 $(PREFIX)/man/man1
	chmod a+r $(PREFIX)/man/man1/bzip2.1
	cp -f bzlib.h $(PREFIX)/include
	chmod a+r $(PREFIX)/include/bzlib.h
	cp -f libbz2.a $(PREFIX)/lib
	chmod a+r $(PREFIX)/lib/libbz2.a

clean: 
	rm -f *.o libbz2.a bzip2 bzip2recover \
	sample1.rb2 sample2.rb2 sample3.rb2 \
	sample1.tst sample2.tst sample3.tst

blocksort.o: blocksort.c
	@cat words0
	$(CC) $(MYCFLAGS) -c blocksort.c $(LDFLAGS)
huffman.o: huffman.c
	$(CC) $(MYCFLAGS) -c huffman.c $(LDFLAGS)
crctable.o: crctable.c
	$(CC) $(MYCFLAGS) -c crctable.c $(LDFLAGS)
randtable.o: randtable.c
	$(CC) $(MYCFLAGS) -c randtable.c $(LDFLAGS)
compress.o: compress.c
	$(CC) $(MYCFLAGS) -c compress.c $(LDFLAGS)
decompress.o: decompress.c
	$(CC) $(MYCFLAGS) -c decompress.c $(LDFLAGS)
bzlib.o: bzlib.c
	$(CC) $(MYCFLAGS) -c bzlib.c $(LDFLAGS)
bzip2.o: bzip2.c
	$(CC) $(MYCFLAGS) -c bzip2.c $(LDFLAGS)
bzip2recover.o: bzip2recover.c
	$(CC) $(MYCFLAGS) -c bzip2recover.c $(LDFLAGS)

DISTNAME=bzip2-1.0.1
tarfile:
	rm -f $(DISTNAME)
	ln -sf . $(DISTNAME)
	tar cvf $(DISTNAME).tar \
	   $(DISTNAME)/blocksort.c \
	   $(DISTNAME)/huffman.c \
	   $(DISTNAME)/crctable.c \
	   $(DISTNAME)/randtable.c \
	   $(DISTNAME)/compress.c \
	   $(DISTNAME)/decompress.c \
	   $(DISTNAME)/bzlib.c \
	   $(DISTNAME)/bzip2.c \
	   $(DISTNAME)/bzip2recover.c \
	   $(DISTNAME)/bzlib.h \
	   $(DISTNAME)/bzlib_private.h \
	   $(DISTNAME)/Makefile \
	   $(DISTNAME)/manual.texi \
	   $(DISTNAME)/manual.ps \
	   $(DISTNAME)/LICENSE \
	   $(DISTNAME)/bzip2.1 \
	   $(DISTNAME)/bzip2.1.preformatted \
	   $(DISTNAME)/bzip2.txt \
	   $(DISTNAME)/words0 \
	   $(DISTNAME)/words1 \
	   $(DISTNAME)/words2 \
	   $(DISTNAME)/words3 \
	   $(DISTNAME)/sample1.ref \
	   $(DISTNAME)/sample2.ref \
	   $(DISTNAME)/sample3.ref \
	   $(DISTNAME)/sample1.bz2 \
	   $(DISTNAME)/sample2.bz2 \
	   $(DISTNAME)/sample3.bz2 \
	   $(DISTNAME)/dlltest.c \
	   $(DISTNAME)/*.html \
	   $(DISTNAME)/README \
	   $(DISTNAME)/README.COMPILATION.PROBLEMS \
	   $(DISTNAME)/CHANGES \
	   $(DISTNAME)/libbz2.def \
	   $(DISTNAME)/libbz2.dsp \
	   $(DISTNAME)/dlltest.dsp \
	   $(DISTNAME)/makefile.msc \
	   $(DISTNAME)/Y2K_INFO \
	   $(DISTNAME)/unzcrash.c \
	   $(DISTNAME)/spewG.c \
	   $(DISTNAME)/Makefile-libbz2_so
