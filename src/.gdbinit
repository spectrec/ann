define qemu
	if $argc != 1
		target remote localhost:1234
	else
		target remote $arg0
	end
end
document qemu
Syntax: qemu [host:port]
| Connect to qemu. Use `localhost:1234' if nothing specified
end

define debug-loader
	set architecture i8086

	add-symbol-file obj/boot/bootloader.debug 0x7c00
	add-symbol-file obj/loader/loader 0x100000

	layout src
end
document debug-loader
Syntax: debug-loader
| Prepare for loader debugging
end

define debug-kernel
	set architecture i386:x86-64

	add-symbol-file obj/kernel/kernel 0xfffffff800000000

	layout src
end
document debug-kernel
Syntax: debug-kernel
| Prepare for kernel debugging
end
