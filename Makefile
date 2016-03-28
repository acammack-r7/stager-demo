CFLAGS = -Os -fPIE
LDFLAGS = -shared -Wl,-Bstatic -Wl,-Bsymbolic

all: test.bin loader

all32: test32.bin loader32

test.bin: elf2bin test
	./elf2bin test test.bin

test: musl test.c
	./musl/bin/musl-gcc -o test musl/lib/rcrt1.o test.c $(CFLAGS) $(LDFLAGS)

loader: musl loader.c
	./musl/bin/musl-gcc -g -o loader loader.c $(CFLAGS)

elf2bin: musl elf2bin.c
	./musl/bin/musl-gcc -g -o elf2bin elf2bin.c $(CFLAGS)

test32.bin: elf2bin32 test
	./elf2bin32 test test32.bin

loader32: musl loader32.c
	./musl/bin/musl-gcc -g -o loader32 loader32.c $(CFLAGS)

elf2bin32: musl elf2bin32.c
	./musl/bin/musl-gcc -g -o elf2bin32 elf2bin32.c $(CFLAGS)

musl: deps/musl/config.mak
	$(MAKE) -C deps/musl install

deps/musl/config.mak:
	cd deps/musl && CFLAGS="-fPIE" ./configure --prefix="`pwd`/../../musl"

clean: clean-test

really-clean: clean clean-musl

distclean: really-clean distclean-musl

clean-musl:
	$(MAKE) -C deps/musl clean
	rm -rf musl

distclean-musl:
	$(MAKE) -C deps/musl distclean

clean-test:
	rm -f test test.bin loader elf2bin

.PHONY: musl clean really-clean distclean clean-musl distclean-musl clean-test
