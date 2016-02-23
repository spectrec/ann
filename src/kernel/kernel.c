void kernel_main(void)
{
	while (1) {
		(*(unsigned *)0xb8000)++;
	}
}
