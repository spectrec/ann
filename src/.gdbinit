target remote localhost:1234
set architecture i8086

add-symbol-file obj/boot/bootloader.debug 0x7c00
add-symbol-file obj/loader/loader 0x100000

layout src
