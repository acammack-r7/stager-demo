CFLAGS = -Os -fPIE
LDFLAGS = -shared -Wl,-Bstatic -Wl,-Bsymbolic

all: test.bin loader

test.bin: elf2bin test
	./elf2bin test test.bin

test: musl test.c
	./musl/bin/musl-gcc -o test musl/lib/rcrt1.o test.c $(CFLAGS) $(LDFLAGS)

loader: musl loader.c
	./musl/bin/musl-gcc -g -o loader loader.c $(CFLAGS)

elf2bin: musl elf2bin.c
	./musl/bin/musl-gcc -g -o elf2bin elf2bin.c $(CFLAGS)

musl: deps/musl/config.mak
	$(MAKE) -C deps/musl install

deps/musl/config.mak:
	cd deps/musl && CFLAGS="-fPIE" ./configure --prefix="`realpath ../../musl`"

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
