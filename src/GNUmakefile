all: Makefile
	$(MAKE) -f Makefile

Makefile: configure
	./configure

configure: configure.ac
	./autogen.sh

lab1: configure drop
	./configure --enable-lab=1
	$(MAKE) -f Makefile

lab2: configure drop
	./configure --enable-lab=2
	$(MAKE) -f Makefile

drop: clean
	rm -f Makefile
	rm -f user/Makefile
	rm -f kernel/Makefile
	rm -f stdlib/Makefile
	rm -f kernel/lib/Makefile
	rm -f kernel/boot/Makefile
	rm -f kernel/loader/Makefile

clean: Makefile
	$(MAKE) -f Makefile clean


# qemu-specific
qemu-gdb: Makefile
	$(MAKE) -f Makefile $@

qemu: Makefile
	$(MAKE) -f Makefile $@

qemu-no-reboot: Makefile
	$(MAKE) -f Makefile $@


# to make `automake' happy
all-am:
clean-am:

.PHONY: all drop clean lab1 lab2
