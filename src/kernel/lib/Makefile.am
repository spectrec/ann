noinst_LIBRARIES = libkernel32.a libkernel64.a

AM_CPPFLAGS = -I${abs_top_srcdir}
SOURCES = memory/map.c disk/ata.c console/terminal.c

libkernel32_a_SOURCES = ${SOURCES}
libkernel32_a_CFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS32@
libkernel32_a_CPPFLAGS = @COMMON_CPPFLAGS@ ${AM_CPPFLAGS} \
			 -DKERNEL_BASE=@KERNEL_BASE@ -DVADDR_BASE=0

libkernel64_a_SOURCES = ${SOURCES}
libkernel64_a_CFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS64@
libkernel64_a_CPPFLAGS = @COMMON_CPPFLAGS@ ${AM_CPPFLAGS} \
			 -DVADDR_BASE=@KERNEL_BASE@ -DKERNEL_BASE=@KERNEL_BASE@
