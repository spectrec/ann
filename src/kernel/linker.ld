ENTRY(kernel_main)

SECTIONS
{
	/* Start from -32GB */
	/*. = 0xFFFFFFF800000000; FIXME*/
	. = 0xF0100000;

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

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	/* Make `end' page aligned */
	. = ALIGN(4K);
	PROVIDE(end = .);
}
