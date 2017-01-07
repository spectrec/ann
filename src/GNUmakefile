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

lab3: configure drop
	./configure --enable-lab=3
	$(MAKE) -f Makefile

lab4: configure drop
	./configure --enable-lab=4
	$(MAKE) -f Makefile

lab5: configure drop
	./configure --enable-lab=5
	$(MAKE) -f Makefile

lab6: configure drop
	./configure --enable-lab=6
	$(MAKE) -f Makefile

lab7: configure drop
	./configure --enable-lab=7
	$(MAKE) -f Makefile

lab8: configure drop
	./configure --enable-lab=8
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

.PHONY: all drop clean lab1 lab2 lab3 lab4 lab5 lab6 lab7 lab8
