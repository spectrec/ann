target remote localhost:1234

#set architecture i8086

#add-symbol-file obj/boot/bootloader.debug 0x7c00
#add-symbol-file obj/loader/loader 0x100000
#add-symbol-file obj/kernel/kernel 0xF0100000

#set architecture i386:x86-64
#add-symbol-file obj/loader/kernel 0xfffffff800000000

#layout src
