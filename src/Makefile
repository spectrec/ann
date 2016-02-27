CFLAGS ?=
LDFLAGS ?=

GCC_PREFIX_X86=i686-pc-linux-gnu-
GCC_PREFIX_X64=x86_64-pc-linux-gnu-

GCC_X64=${GCC_PREFIX_X64}gcc
AS_X64=${GCC_PREFIX_X64}as
LD_X64=${GCC_PREFIX_X64}ld

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

LIB_DIR := lib
LOADER_DIR := loader
KERNEL_DIR := kernel
BOOTLOADER_DIR := boot

OBJDIR := obj
KERNEL_OBJDIR := ${OBJDIR}/${KERNEL_DIR}
LOADER_OBJDIR := ${OBJDIR}/${LOADER_DIR}
BOOTLOADER_OBJDIR := ${OBJDIR}/${BOOTLOADER_DIR}

IMAGE := disk.img
LOADER := ${LOADER_OBJDIR}/loader
KERNEL := ${KERNEL_OBJDIR}/kernel
BOOTLOADER := ${BOOTLOADER_OBJDIR}/bootloader

all: dirs ${IMAGE}

dirs:
	@mkdir -p ${BOOTLOADER_OBJDIR}
	@mkdir -p ${LOADER_OBJDIR}
	@mkdir -p ${KERNEL_OBJDIR}

# Image specific
${IMAGE}: $(BOOTLOADER) $(LOADER) $(KERNEL)
	dd if=/dev/zero of=${IMAGE} bs=1M count=10
	dd if=$(BOOTLOADER) of=${IMAGE} conv=notrunc
	dd if=$(LOADER) of=${IMAGE} seek=1 conv=notrunc
	dd if=$(KERNEL) of=${IMAGE} bs=1M seek=1 conv=notrunc


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
		  ${LOADER_OBJDIR}/mmap.o ${LOADER_OBJDIR}/ata.o \
		  ${LOADER_OBJDIR}/string.o ${LOADER_OBJDIR}/long_mode.o
LOADER_DEPS := $(patsubst %.o,%.d,${LOADER_OBJCECTS})

$(LOADER): ${LOADER_OBJECTS}
	$(GCC_X86) -T ${LOADER_DIR}/linker.ld -o $@ $^ ${LOADER_LDFLAGS}

${LOADER_OBJDIR}/entry.o: ${LOADER_DIR}/entry.S
	$(AS_X86) -o $@ $^

${LOADER_OBJDIR}/long_mode.o: ${LOADER_DIR}/long_mode.S
	$(AS_X86) -o $@ $^

${LOADER_OBJDIR}/string.o: ${LIB_DIR}/string.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/mmap.o: ${LIB_DIR}/mm/mmap.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/loader.o: ${LOADER_DIR}/loader.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/ata.o: ${LIB_DIR}/fs/ata.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

-include ${LOADER_DEPS}


# Kernel specific
KERNEL_CFLAGS := ${CFLAGS} -O0 -DVADDR_BASE=0xFFFFFFF800000000 -mcmodel=large \
		 -mno-red-zone -mno-mmx -mno-sse -mno-sse2
KERNEL_LDFLAGS := ${LDFLAGS} -O0 -lgcc

KERNEL_OBJECTS := ${KERNEL_OBJDIR}/kernel.o ${KERNEL_OBJDIR}/terminal.o \
		  ${KERNEL_OBJDIR}/string.o
KERNEL_DEPS := $(patsubst %.o,%.d,${KERNEL_OBJECTS})

$(KERNEL): ${KERNEL_OBJECTS}
	$(GCC_X64) -T ${KERNEL_DIR}/linker.ld -o $@ $^ ${KERNEL_LDFLAGS}

${KERNEL_OBJDIR}/kernel.o: ${KERNEL_DIR}/kernel.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/string.o: ${LIB_DIR}/string.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/terminal.o: ${LIB_DIR}/console/terminal.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

-include ${KERNEL_DEPS}


# Qemu specific
qemu: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -s -S -m 32M

clean:
	rm -rf ${OBJDIR} ${IMAGE}

.PHONY: clean dirs
