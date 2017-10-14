ENTRY(_start)

SECTIONS
{
	/* Start from 1MB */
	. = 1M;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
		*(.long_mode_asm)
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
