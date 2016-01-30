GCCPREFIX=i686-pc-linux-gnu

GCC=$(GCCPREFIX)-gcc
LD=$(GCCPREFIX)-ld
OBJCOPY=$(GCCPREFIX)-objcopy
PERL=perl

CFLAGS += -Wall -Werror -Wextra -std=gnu99 -ffreestanding -nostdlib -Os -g -I.
LDFLAGS += -ffreestanding -Os -nostdlib -g

bootloader: boot.o main.o ata.o
	$(LD) -o $@.out --entry=boot_entry -Ttext 0x7c00 $^
	$(OBJCOPY) --only-keep-debug $@.out $@.debug
	$(OBJCOPY) -S -O binary -j .text $@.out $@.bin
	$(PERL) sign.pl $@.bin

boot.o: boot.S
	$(GCC) -c -o $@ ${CFLAGS} $^

%.o: %.c
	$(GCC) -c -o $@ ${CFLAGS} $^

clean:
	rm -f *.{o,out,bin,debug}
