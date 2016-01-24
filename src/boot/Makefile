GCCPREFIX=i686-pc-linux-gnu

GCC=$(GCCPREFIX)-gcc
LD=$(GCCPREFIX)-ld
OBJCOPY=$(GCCPREFIX)-objcopy
PERL=perl

CFLAGS += -std=gnu99 -ffreestanding -Os -nostdlib -g -I.
LDFLAGS += -ffreestanding -Os -nostdlib -g

bootloader: boot.o main.o
	$(LD) -o $@.out --entry=boot_entry -Ttext 0x7c00 $^
	$(OBJCOPY) --only-keep-debug $@.out $@.debug
	$(OBJCOPY) -S -O binary -j .text $@.out $@.bin
	$(PERL) sign.pl $@.bin

boot.o: boot.S
	$(GCC) -c -o $@ ${CFLAGS} $^

main.o: main.c
	$(GCC) -c -o $@ ${CFLAGS} $^

clean:
	rm -f *.{o,out,bin,debug}
