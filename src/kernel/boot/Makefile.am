bin_PROGRAMS = bootloader

bootloader_SOURCES = boot.S main.c ../lib/disk/ata.c

bootloader_CPPFLAGS = @COMMON_CPPFLAGS@ -I$(abs_top_srcdir)
bootloader_CFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS32@ -Os
bootloader_CCASFLAGS = @COMMON_CFLAGS@ @EXTRA_CFLAGS32@ -Os
bootloader_LDFLAGS = @COMMON_LDFLAGS@ -Wl,--entry=boot_entry -Wl,-Ttext -Wl,0x7c00

all-local: bootloader
	$(OBJCOPY) --only-keep-debug $^ $^.debug
	$(OBJCOPY) -S -O binary -j .text $^ $^.strip
	$(PERL) sign.pl $^.strip

clean-local:
	rm -f *.debug
	rm -f *.strip
	rm -rf lib
