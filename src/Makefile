CFLAGS ?=
LDFLAGS ?=

GCC_PREFIX_X86=i686-pc-linux-gnu-
GCC_X86=${GCC_PREFIX_X86}gcc
LD_X86=${GCC_PREFIX_X86}ld
OBJCOPY_X86=${GCC_PREFIX_X86}objcopy
PERL=perl

# Common `CFLAGS' and `LDFLAGS'
CFLAGS += -Wall -Wextra -Werror		\
	  -ffreestanding -nostdlib	\
	  -std=gnu11 -g -Iinclude -Ilib	\
	  -MMD -MP
LDFLAGS += -ffreestanding -nostdlib -g

OBJDIR := obj

LIB_DIR := lib

BOOTLOADER_CFLAGS := ${CFLAGS} -Os
BOOTLOADER_LDFLAGS := ${LDFLAGS} -Os

BOOTLOADER_DIR := boot
BOOTLOADER_OBJDIR := ${OBJDIR}/${BOOTLOADER_DIR}
BOOTLOADER_OBJCECTS := ${BOOTLOADER_OBJDIR}/main.o ${BOOTLOADER_OBJDIR}/ata.o ${BOOTLOADER_OBJDIR}/entry.o
BOOTLOADER_DEPS := $(patsubst %.o,%.d,${BOOTLOADER_OBJCECTS})
BOOTLOADER := ${BOOTLOADER_OBJDIR}/bootloader

all: $(BOOTLOADER)

${BOOTLOADER_OBJDIR}/ata.o: ${LIB_DIR}/fs/ata.c
	@mkdir -p $(@D)
	$(GCC_X86) -c -o $@ $< ${BOOTLOADER_CFLAGS}

${BOOTLOADER_OBJDIR}/main.o: ${BOOTLOADER_DIR}/main.c
	@mkdir -p $(@D)
	$(GCC_X86) -c -o $@ $< ${BOOTLOADER_CFLAGS}

${BOOTLOADER_OBJDIR}/entry.o: ${BOOTLOADER_DIR}/boot.S
	@mkdir -p $(@D)
	$(GCC_X86) -c -o $@ $< ${BOOTLOADER_CFLAGS}

$(BOOTLOADER): ${BOOTLOADER_OBJCECTS}
	$(LD_X86) -o $@ --entry=boot_entry -Ttext 0x7c00 $^
	$(OBJCOPY_X86) --only-keep-debug $@ $@.debug
	$(OBJCOPY_X86) -S -O binary -j .text $@ $@.strip
	$(PERL) ${BOOTLOADER_DIR}/sign.pl $@.strip
	mv $@.strip $@

-include ${BOOTLOADER_DEPS}

clean:
	rm -rf ${OBJDIR}

.PHONY: clean
