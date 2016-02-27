#include <assert.h>
#include <console/terminal.h>

panic_t panic;
void kernel_panic(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	terminal_vprintf(fmt, ap);
	va_end(ap);

	while (1) {
		/*do nothing*/;
	}
}

void kernel_main(void)
{
	terminal_init();

	// Initialize assert
	panic = kernel_panic;

	panic("Nothing to do");
}
