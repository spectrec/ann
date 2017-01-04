SUBDIRS = boot lib loader

USER_PROGS = $(wildcard $(top_srcdir)/user/*.bin)

bin_PROGRAMS = kernel
kernel_SOURCES = kernel.c \
		 syscall.c \
		 cpu.c \
		 task.c \
		 thread.c \
		 monitor.c \
		 interrupt/interrupt.c \
		 interrupt/timer.c \
		 interrupt/keyboard.c \
		 interrupt/interrupt_entry.S

kernel_CFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS64@
kernel_CPPFLAGS = @COMMON_CPPFLAGS@ -I$(abs_top_srcdir) \
		  -DVADDR_BASE=@KERNEL_BASE@ -DKERNEL_BASE=@KERNEL_BASE@
kernel_LDADD = $(abs_top_builddir)/stdlib/libstd64.a \
	       $(abs_top_builddir)/kernel/lib/libkernel64.a -lgcc
kernel_LDFLAGS = -T linker.ld -Wl,--format=binary -Wl,${USER_PROGS} -Wl,--format=elf64-x86-64
