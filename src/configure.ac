AC_PREREQ([2.69])

AC_INIT([Ann], [1.0], [belous.vs@yandex.ru])
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Disable default `-O2 -g'
if test "x$CFLAGS" = "x"; then
	CFLAGS=""
fi

# checks for programms
AC_CHECK_TOOL([OBJCOPY], [objcopy], [:])
AC_CHECK_PROGS([GDB], [gdb], [AC_MSG_ERROR([gdb not found])])
AC_CHECK_PROGS([PERL], [perl], [AC_MSG_ERROR([perl not found])])
AC_CHECK_PROGS([QEMU], [qemu-system-x86_64], [AC_MSG_ERROR([qemu not found])])

AM_PROG_AS
AC_PROG_CC
AC_PROG_RANLIB

# setup config files
AC_LANG([C])

AC_CONFIG_FILES([
	Makefile
	user/Makefile
	stdlib/Makefile
	kernel/Makefile
	kernel/lib/Makefile
	kernel/boot/Makefile
	kernel/loader/Makefile
])

# setup flags
COMMON_CFLAGS="-Wall -Wextra -Werror -std=gnu11 -ffreestanding -nostdlib -ggdb3"
AC_SUBST([COMMON_CFLAGS])

EXTRA_CFLAGS64="-m64 -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
AC_SUBST([EXTRA_CFLAGS64])

EXTRA_CFLAGS32="-m32"
AC_SUBST([EXTRA_CFLAGS32])

COMMON_LDFLAGS="-ggdb3"
AC_SUBST([COMMON_LDFLAGS])

AC_ARG_ENABLE([lab], [enable lab 'N'], [], [enable_lab="1"])
COMMON_CPPFLAGS="-DLAB=$enable_lab"
AC_SUBST([COMMON_CPPFLAGS])

# and some common variables
KERNEL_BASE=0xFFFFFFF800000000
AC_SUBST([KERNEL_BASE])

AC_OUTPUT
