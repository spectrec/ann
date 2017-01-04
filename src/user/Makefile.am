bin_PROGRAMS = hello.bin \
	       fork.bin \
	       spin.bin \
	       exit.bin \
	       read_kernel.bin \
	       read_unmap.bin \
	       write_kernel.bin \
	       write_unmap.bin \
	       yield.bin

AM_CFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS64@
AM_LDFLAGS = @COMMON_LDFLAGS@ -T linker.ld -lgcc
AM_CPPFLAGS = @COMMON_CPPFLAGS@ -D__USER__ -I$(abs_top_srcdir)

noinst_LIBRARIES = libcommon.a
libcommon_a_SOURCES = entry.c syscall.c

hello_bin_SOURCES = hello.c
hello_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a

fork_bin_SOURCES = fork.c
fork_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a

spin_bin_SOURCES = spin.c
spin_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a

exit_bin_SOURCES = exit.c
exit_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a

read_kernel_bin_SOURCES = read_kernel.c
read_kernel_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a

read_unmap_bin_SOURCES = read_unmap.c
read_unmap_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a

write_kernel_bin_SOURCES = write_kernel.c
write_kernel_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a

write_unmap_bin_SOURCES = write_unmap.c
write_unmap_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a

yield_bin_SOURCES = yield.c
yield_bin_LDADD = libcommon.a $(abs_top_builddir)/stdlib/libstd64.a
