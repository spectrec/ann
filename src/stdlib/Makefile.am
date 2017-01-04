noinst_LIBRARIES = libstd32.a libstd64.a

AM_CPPFLAGS = -I${abs_top_srcdir}
SOURCES = string.c stdlib.c

libstd32_a_SOURCES = ${SOURCES}
libstd32_a_CFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS32@
libstd32_a_CPPFLAGS = @COMMON_CPPFLAGS@ ${AM_CPPFLAGS}

libstd64_a_SOURCES = ${SOURCES}
libstd64_a_CFLAGS = @COMMON_CFLAGS@
libstd64_a_CPPFLAGS = @COMMON_CPPFLAGS@ ${AM_CPPFLAGS}
