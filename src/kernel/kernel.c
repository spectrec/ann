void kernel_main(void)
{
	char *memory = (char *)0xb8000;
	memory[0] = 'k'; memory[0] = 'k';
	memory[1] = 'e'; memory[1] = 'e';
	memory[2] = 'r'; memory[2] = 'r';
	memory[3] = 'n'; memory[3] = 'n';
	memory[4] = 'e'; memory[4] = 'e';
	memory[5] = 'l'; memory[5] = 'l';

	while (1) {
		/*do nothing*/;
	}
}
