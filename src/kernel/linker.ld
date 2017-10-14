ENTRY(kernel_main)

SECTIONS
{
	/* Start from -32GB + 2MB */
	. = 0xFFFFFFF800200000;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	PROVIDE(edata = .);

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	PROVIDE(end = .);

	/* pass it at the end to avoid problems with debugging symbols
	 *  see `--build-id' linker option for more information (another way
	 *  to fix it is to append `-Wl,--build-id=none' into all ldflags)*/
	.note.gnu.build-id : {
	}
}
