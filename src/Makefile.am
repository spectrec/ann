SUBDIRS = stdlib user kernel

BOOTLOADER = kernel/boot/bootloader.strip
LOADER = kernel/loader/loader
KERNEL = kernel/kernel

IMAGE = kernel.img

${IMAGE}: all
	dd if=/dev/zero of=${IMAGE} bs=1M count=40
	dd if=$(BOOTLOADER) of=${IMAGE} conv=notrunc
	dd if=$(LOADER) of=${IMAGE} seek=1 conv=notrunc
	dd if=$(KERNEL) of=${IMAGE} bs=1M seek=1 conv=notrunc

qemu-gdb: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -s -S

qemu: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -d int,cpu_reset,unimp

qemu-no-reboot: ${IMAGE}
	$(QEMU) -drive file=$<,index=0,media=disk,format=raw -no-reboot -no-shutdown -d int,cpu_reset,unimp

clean-local:
	rm -f ${IMAGE}

.PHONY: qemu-gdb qemu qemu-no-reboot
