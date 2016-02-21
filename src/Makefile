CFLAGS ?=
LDFLAGS ?=

GCC_PREFIX_X86=i686-pc-linux-gnu-
GCC_X86=${GCC_PREFIX_X86}gcc
LD_X86=${GCC_PREFIX_X86}ld
AS_X86=${GCC_PREFIX_X86}as
OBJCOPY_X86=${GCC_PREFIX_X86}objcopy
QEMU=qemu-system-x86_64
PERL=perl

# Common `CFLAGS' and `LDFLAGS'
CFLAGS += -Wall -Wextra -Werror		\
	  -ffreestanding -nostdlib	\
	  -std=gnu11 -g -Iinclude -Ilib	\
	  -MMD -MP
LDFLAGS += -ffreestanding -nostdlib -g

OBJDIR := obj
LIB_DIR := lib
LOADER_DIR := loader
BOOTLOADER_DIR := boot

LOADER_OBJDIR := ${OBJDIR}/${LOADER_DIR}
BOOTLOADER_OBJDIR := ${OBJDIR}/${BOOTLOADER_DIR}

IMAGE := disk.img
LOADER := ${LOADER_OBJDIR}/loader
BOOTLOADER := ${BOOTLOADER_OBJDIR}/bootloader

all: dirs ${IMAGE}

dirs:
	@mkdir -p ${BOOTLOADER_OBJDIR}
	@mkdir -p ${LOADER_OBJDIR}

# Image specific
${IMAGE}: $(BOOTLOADER) $(LOADER)
	dd if=/dev/zero of=${IMAGE} bs=1M count=10
	dd if=$(BOOTLOADER) of=${IMAGE} conv=notrunc
	dd if=$(LOADER) of=${IMAGE} seek=1 conv=notrunc

# Bootloader specific
BOOTLOADER_CFLAGS := ${CFLAGS} -Os
BOOTLOADER_LDFLAGS := --entry=boot_entry -Ttext 0x7c00

# XXX: order is important (`entry.o' must be first, otherwise nothing will work)
BOOTLOADER_OBJCECTS := ${BOOTLOADER_OBJDIR}/entry.o ${BOOTLOADER_OBJDIR}/main.o ${BOOTLOADER_OBJDIR}/ata.o
BOOTLOADER_DEPS := $(patsubst %.o,%.d,${BOOTLOADER_OBJCECTS})

${BOOTLOADER_OBJDIR}/ata.o: ${LIB_DIR}/fs/ata.c
	$(GCC_X86) -c -o $@ $< ${BOOTLOADER_CFLAGS}

${BOOTLOADER_OBJDIR}/main.o: ${BOOTLOADER_DIR}/main.c
	$(GCC_X86) -c -o $@ $< ${BOOTLOADER_CFLAGS}

${BOOTLOADER_OBJDIR}/entry.o: ${BOOTLOADER_DIR}/boot.S
	$(GCC_X86) -c -o $@ $< ${BOOTLOADER_CFLAGS}

$(BOOTLOADER): ${BOOTLOADER_OBJCECTS}
	$(LD_X86) -o $@ ${BOOTLOADER_LDFLAGS} $^
	$(OBJCOPY_X86) --only-keep-debug $@ $@.debug
	$(OBJCOPY_X86) -S -O binary -j .text $@ $@.strip
	$(PERL) ${BOOTLOADER_DIR}/sign.pl $@.strip
	mv $@.strip $@

-include ${BOOTLOADER_DEPS}

# Loader specific
LOADER_CFLAGS := ${CFLAGS} -O0
LOADER_LDFLAGS := ${LDFLAGS} -O0 -lgcc

LOADER_OBJECTS := ${LOADER_OBJDIR}/entry.o ${LOADER_OBJDIR}/loader.o \
		  ${LOADER_OBJDIR}/mmap.o
LOADER_DEPS := $(patsubst %.o,%.d,${LOADER_OBJCECTS})

$(LOADER): ${LOADER_OBJECTS}
	$(GCC_X86) -T ${LOADER_DIR}/linker.ld -o $@ $^ ${LOADER_LDFLAGS}

${LOADER_OBJDIR}/entry.o: ${LOADER_DIR}/entry.S
	$(AS_X86) -o $@ $^

${LOADER_OBJDIR}/string.o: ${LIB_DIR}/string.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/mmap.o: ${LIB_DIR}/mm/mmap.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/loader.o: ${LOADER_DIR}/loader.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

-include ${LOADER_DEPS}

# Qemu specific
qemu: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -s -S -m 32M

clean:
	rm -rf ${OBJDIR} ${IMAGE}

.PHONY: clean dirs
