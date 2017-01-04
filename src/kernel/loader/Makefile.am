bin_PROGRAMS = loader

loader_CFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS32@
loader_CCASFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS32@
loader_CPPFLAGS = @COMMON_CPPFLAGS@ -I${abs_top_srcdir} \
		  -DKERNEL_BASE=${KERNEL_BASE} -DVADDR_BASE=0
loader_LDADD = $(abs_top_builddir)/kernel/lib/libkernel32.a \
	       $(abs_top_builddir)/stdlib/libstd32.a -lgcc
loader_LDFLAGS = -T linker.ld

loader_SOURCES = loader.c entry.S long_mode.S
