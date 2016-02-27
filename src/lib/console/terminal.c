#include <string.h>
#include <assert.h>

#include "terminal.h"

#define TERMINAL_ROW_COUNT	25
#define TERMINAL_COL_COUNT	80

#define TERMINAL_ROW_SIZE (TERMINAL_COL_COUNT * sizeof(uint16_t))
#define TERMINAL_ROW(i_) (&terminal_buffer[(i_) * TERMINAL_ROW_SIZE])

static uint16_t *terminal_buffer;
static uint8_t terminal_color;
static size_t terminal_column;
static size_t terminal_row;

static uint16_t terminal_make_char(uint8_t ch, uint8_t color)
{
	return ((uint16_t)color << 8) | ch;
}

uint8_t terminal_make_color(enum terminal_color fg, enum terminal_color bg)
{
	return (bg << 4) | (fg & 0xf);
}

#define TERMINAL_TAB_SPACE	8
void terminal_put_color(uint8_t ch, uint8_t color)
{
	size_t index = terminal_row*TERMINAL_COL_COUNT + terminal_column;

	switch (ch) {
	case '\t':
		for (uint8_t i = 0; i < TERMINAL_TAB_SPACE; i++)
			terminal_put_color(' ', color);

		return;
	case '\r':
		terminal_column = 0;
		return;
	case '\n':
		terminal_column = 0;
		terminal_row++;
		break;
	default:
		terminal_buffer[index] = terminal_make_char(ch, color);
		terminal_column++;
	}

	assert(terminal_column <= TERMINAL_COL_COUNT);
	if (terminal_column == TERMINAL_COL_COUNT) {
		terminal_column = 0;
		terminal_row++;
	}

	assert(terminal_row <= TERMINAL_ROW_COUNT);
	if (terminal_row == TERMINAL_ROW_COUNT) {
		for (uint8_t i = 0; i < TERMINAL_ROW_COUNT-1; i++)
			memcpy(TERMINAL_ROW(i), TERMINAL_ROW(i+1), TERMINAL_ROW_SIZE);

		terminal_row = TERMINAL_ROW_COUNT - 1;
		memset(TERMINAL_ROW(terminal_row), 0, TERMINAL_ROW_SIZE);

		assert(terminal_column == 0);
	}
}

void terminal_put(uint8_t ch)
{
	terminal_put_color(ch, terminal_color);
}

static uint8_t convert_digit(uint8_t digit, uint8_t base)
{
	uint8_t digit2hex[] = "0123456789ABCDEF";
	assert(digit < base);

	if (digit < 10)
		return digit2hex[digit];

	assert(base == 16);
	return digit2hex[digit];
}

static void terminal_print_digit(uint64_t digit, uint8_t base)
{
	assert(base == 2 || base == 10 || base == 16);

	if (digit < base)
		return terminal_put(convert_digit((uint8_t)digit, base));

	terminal_print_digit(digit / base, base);
	terminal_put(convert_digit((uint8_t)(digit % base), base));
}

void terminal_vprintf(const char *fmt, va_list ap)
{
	while (*fmt != '\0') {
		if (*fmt != '%') {
			terminal_put(*fmt++);
			continue;
		}

		switch (*(++fmt)) {
		case '%':
			terminal_put('%');
			break;
		case 'c': {
			char c = va_arg(ap, int);
			terminal_put(c);

			break;
		}
		case 's': {
			const char *s = va_arg(ap, const char *);
			while (*s != '\0')
				terminal_put(*s++);

			break;
		}
		case 'd': {
			int32_t digit = va_arg(ap, int32_t);
			if (digit < 0) {
				terminal_put('-');
				digit = -digit;
			}

			terminal_print_digit((uint64_t)digit, 10);
			break;
		}
		case 'u': {
			uint32_t digit = va_arg(ap, uint32_t);
			terminal_print_digit((uint64_t)digit, 10);
			break;
		}
		case 'b': {
			uint32_t digit = va_arg(ap, uint32_t);
			terminal_print_digit((uint64_t)digit, 2);
			break;
		}
		case 'x': {
			uint32_t digit = va_arg(ap, uint32_t);
			terminal_printf("0x");
			terminal_print_digit((uint64_t)digit, 16);
			break;
		}
		case 'p': {
			uintptr_t digit = va_arg(ap, uintptr_t);
			terminal_printf("0x");
			terminal_print_digit((uint64_t)digit, 16);
			break;
		}
		default:
			panic("\nformat `%c' not implemented", *fmt);
		}

		fmt++;
	}
}

void terminal_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	terminal_vprintf(fmt, ap);
	va_end(ap);
}

void terminal_clear(void)
{
	terminal_color = terminal_make_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
	terminal_column = terminal_row = 0;

	terminal_buffer = (uint16_t *)0xb8000;
	memset(terminal_buffer, 0, TERMINAL_ROW_COUNT * TERMINAL_ROW_SIZE);
}

void terminal_init(void)
{
	terminal_clear();
}
