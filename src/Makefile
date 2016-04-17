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

# Where to load kernel (-32Gb)
KERNEL_BASE := 0xFFFFFFF800000000

# Common `CFLAGS' and `LDFLAGS'
CFLAGS += -Wall -Wextra -Werror			\
	  -ffreestanding -nostdlib		\
	  -std=gnu11 -ggdb3 -Iinclude -Ilib	\
	  -MMD -MP
LDFLAGS += -ffreestanding -nostdlib -ggdb3

LIB_DIR := lib
USER_DIR := user
LOADER_DIR := loader
KERNEL_DIR := kernel
BOOTLOADER_DIR := boot

OBJDIR := obj
USER_OBJDIR := ${OBJDIR}/${USER_DIR}
KERNEL_OBJDIR := ${OBJDIR}/${KERNEL_DIR}
LOADER_OBJDIR := ${OBJDIR}/${LOADER_DIR}
BOOTLOADER_OBJDIR := ${OBJDIR}/${BOOTLOADER_DIR}

IMAGE := disk.img
LOADER := ${LOADER_OBJDIR}/loader
KERNEL := ${KERNEL_OBJDIR}/kernel
BOOTLOADER := ${BOOTLOADER_OBJDIR}/bootloader
USER_PROGRAMMS := ${USER_OBJDIR}/hello.bin

all: ${IMAGE}

dirs:
	@mkdir -p ${BOOTLOADER_OBJDIR}
	@mkdir -p ${LOADER_OBJDIR}
	@mkdir -p ${KERNEL_OBJDIR}
	@mkdir -p ${USER_OBJDIR}

# Image specific
${IMAGE}: dirs $(USER_PROGRAMMS) $(BOOTLOADER) $(LOADER) $(KERNEL)
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

${BOOTLOADER_OBJDIR}/ata.o: ${LIB_DIR}/disk/ata.c
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
LOADER_CFLAGS := ${CFLAGS} -O0 -DKERNEL_BASE=${KERNEL_BASE}
LOADER_LDFLAGS := ${LDFLAGS} -O0 -lgcc

LOADER_OBJECTS := ${LOADER_OBJDIR}/entry.o ${LOADER_OBJDIR}/loader.o \
		  ${LOADER_OBJDIR}/mmap.o ${LOADER_OBJDIR}/ata.o \
		  ${LOADER_OBJDIR}/string.o ${LOADER_OBJDIR}/long_mode.o \
		  ${LOADER_OBJDIR}/terminal.o
LOADER_DEPS := $(patsubst %.o,%.d,${LOADER_OBJCECTS})

$(LOADER): ${LOADER_OBJECTS}
	$(GCC_X86) -T ${LOADER_DIR}/linker.ld -o $@ $^ ${LOADER_LDFLAGS}

${LOADER_OBJDIR}/entry.o: ${LOADER_DIR}/entry.S
	$(GCC_X86) -c -o $@ $^ ${LOADER_CFLAGS}

${LOADER_OBJDIR}/long_mode.o: ${LOADER_DIR}/long_mode.S
	$(GCC_X86) -c -o $@ $^ ${LOADER_CFLAGS}

${LOADER_OBJDIR}/string.o: ${LIB_DIR}/string.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/mmap.o: ${LIB_DIR}/mm/mmap.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/loader.o: ${LOADER_DIR}/loader.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/ata.o: ${LIB_DIR}/disk/ata.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

${LOADER_OBJDIR}/terminal.o: ${LIB_DIR}/console/terminal.c
	$(GCC_X86) -c -o $@ $< ${LOADER_CFLAGS}

-include ${LOADER_DEPS}


# Kernel specific
KERNEL_CFLAGS := ${CFLAGS} -O0 -DVADDR_BASE=${KERNEL_BASE} -mcmodel=large \
		 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -I.
KERNEL_LDFLAGS := ${LDFLAGS} -O0 -lgcc -Wl,--format=binary -Wl,${USER_PROGRAMMS} -Wl,--format=default

KERNEL_OBJECTS := ${KERNEL_OBJDIR}/kernel.o ${KERNEL_OBJDIR}/terminal.o \
		  ${KERNEL_OBJDIR}/string.o ${KERNEL_OBJDIR}/mmap.o \
		  ${KERNEL_OBJDIR}/interrupt.o ${KERNEL_OBJDIR}/interrupt_entry.o \
		  ${KERNEL_OBJDIR}/task.o ${KERNEL_OBJDIR}/cpu.o ${KERNEL_OBJDIR}/syscall.o \
		  ${KERNEL_OBJDIR}/keyboard.o ${KERNEL_OBJDIR}/timer.o
KERNEL_DEPS := $(patsubst %.o,%.d,${KERNEL_OBJECTS})

$(KERNEL): ${KERNEL_OBJECTS}
	$(GCC_X64) -T ${KERNEL_DIR}/linker.ld -o $@ $^ ${KERNEL_LDFLAGS}

${KERNEL_OBJDIR}/kernel.o: ${KERNEL_DIR}/kernel.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/timer.o: ${KERNEL_DIR}/timer.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/keyboard.o: ${KERNEL_DIR}/keyboard.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/syscall.o: ${KERNEL_DIR}/syscall.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/cpu.o: ${KERNEL_DIR}/cpu.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/task.o: ${KERNEL_DIR}/task.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/interrupt_entry.o: ${KERNEL_DIR}/interrupt_entry.S
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/interrupt.o: ${KERNEL_DIR}/interrupt.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/mmap.o: ${LIB_DIR}/mm/mmap.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/string.o: ${LIB_DIR}/string.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

${KERNEL_OBJDIR}/terminal.o: ${LIB_DIR}/console/terminal.c
	$(GCC_X64) -c -o $@ $< ${KERNEL_CFLAGS}

-include ${KERNEL_DEPS}


# User specific
USER_CFLAGS := ${CFLAGS} -O0
USER_LDFLAGS := ${LDFLAGS} -O0 -lgcc

USER_DEPS := $(patsubst %.bin,%.d,${USER_PROGRAMMS})

${USER_OBJDIR}/%.o: ${USER_DIR}/%.c
	$(GCC_X64) -c -o $@ $< ${USER_CFLAGS}

${USER_OBJDIR}/%.bin: ${USER_OBJDIR}/%.o ${USER_OBJDIR}/entry.o ${USER_OBJDIR}/syscall.o
	$(GCC_X64) -T ${USER_DIR}/linker.ld -o $@ $^ ${USER_LDFLAGS}

-include ${USER_DEPS}


# Qemu specific
qemu-gdb: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -s -S

qemu: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -d int,cpu_reset,unimp

qemu-no-reboot: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -no-reboot -no-shutdown -d int,cpu_reset,unimp

clean:
	rm -rf ${OBJDIR} ${IMAGE}

.PHONY: clean dirs
