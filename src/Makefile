QEMU = qemu-system-x86_64
PERL = perl

AR = ar
RANLIB = ranlib
OBJCOPY = objcopy

TOP_SRC_DIR = .

IMAGE = kernel.img

# Common `CFLAGS' and `LDFLAGS'
CFLAGS += -Wall -Wextra -Werror -I ${TOP_SRC_DIR}	\
	  -ffreestanding -nostdlib			\
	  -std=gnu11 -ggdb3 -MMD -MP
LDFLAGS += -ffreestanding -nostdlib -ggdb3

X86_CFLAGS = -m32
X86_LDFLAGS = -m32

X64_CFLAGS = -m64 -mcmodel=large -mno-red-zone \
	     -mno-mmx -mno-sse -mno-sse2
X64_LDFLAGS = -m64

ARCH32 = i386
ARCH64 = x86_64

# stdlib specific
STDLIB_DIR = stdlib
STDLIB_NAME = stdlib.a

STDLIB32_CFLAGS = ${CFLAGS} ${X86_CFLAGS}
STDLIB32_DIR = ${STDLIB_DIR}/${ARCH32}

STDLIB64_CFLAGS = ${CFLAGS} ${X64_CFLAGS}
STDLIB64_DIR = ${STDLIB_DIR}/${ARCH64}

STDLIB32 = ${STDLIB32_DIR}/${STDLIB_NAME}
STDLIB64 = ${STDLIB64_DIR}/${STDLIB_NAME}

# user specific
USER_DIR = user
USER_PROGS = ${USER_DIR}/hello.bin \
	     ${USER_DIR}/fork.bin \
	     ${USER_DIR}/spin.bin \
	     ${USER_DIR}/exit.bin \
	     ${USER_DIR}/read_kernel.bin \
	     ${USER_DIR}/read_unmap.bin \
	     ${USER_DIR}/write_kernel.bin \
	     ${USER_DIR}/write_unmap.bin \
	     ${USER_DIR}/yield.bin

# Kernel specific
KERNEL_DIR = kernel

# Make `all' top target
all: ${IMAGE}

# Don't delete intermediate files
.SECONDARY:

STDLIB_ARCH := ${ARCH32}
STDLIB_CFLAGS := ${STDLIB32_CFLAGS}
include ${STDLIB_DIR}/Makefile.part

STDLIB_ARCH := ${ARCH64}
STDLIB_CFLAGS := ${STDLIB64_CFLAGS}
include ${STDLIB_DIR}/Makefile.part

include ${USER_DIR}/Makefile.part

include ${KERNEL_DIR}/Makefile.part

# Qemu specific
qemu-gdb: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -s -S

qemu: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -d int,cpu_reset,unimp

qemu-no-reboot: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -no-reboot -no-shutdown -d int,cpu_reset,unimp

clean:
	rm -rf ${STDLIB32_DIR} ${STDLIB64_DIR}
	rm -rf ${KERNEL_LIB32_DIR} ${KERNEL_LIB64_DIR}
	rm -f ${kernel_deps} ${kernel_objects} $(KERNEL)
	rm -f ${bootloader_deps} ${bootloader_objects} $(BOOTLOADER){,.debug}
	rm -f ${loader_deps} ${loader_objects} $(LOADER)
	rm -f ${USER_DIR}/*.{o,d,bin}
	rm -f ${IMAGE}

.PHONY: clean qemu-gdb qemu qemu-no-reboot all
