#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <stdint.h>
#include <stdarg.h>

enum terminal_color {
	TERMINAL_COLOR_BLACK		= 0,
	TERMINAL_COLOR_BLUE		= 1,
	TERMINAL_COLOR_GREEN		= 2,
	TERMINAL_COLOR_CYAN		= 3,
	TERMINAL_COLOR_RED		= 4,
	TERMINAL_COLOR_MAGENTA		= 5,
	TERMINAL_COLOR_BROWN		= 6,
	TERMINAL_COLOR_LIGHT_GREY	= 7,
	TERMINAL_COLOR_DARK_GREY	= 8,
	TERMINAL_COLOR_LIGHT_BLUE	= 9,
	TERMINAL_COLOR_LIGHT_GREEN	= 10,
	TERMINAL_COLOR_LIGHT_CYAN	= 11,
	TERMINAL_COLOR_LIGHT_RED	= 12,
	TERMINAL_COLOR_LIGHT_MAGENTA	= 13,
	TERMINAL_COLOR_LIGHT_BROWN	= 14,
	TERMINAL_COLOR_WHITE		= 15,
};

uint8_t terminal_make_color(enum terminal_color fg, enum terminal_color bg);

void terminal_put_color(uint8_t ch, uint8_t color);
void terminal_vprintf(const char *fmt, va_list ap);
void terminal_printf(const char *fmt, ...);
void terminal_put(uint8_t ch);

void terminal_clear(void);
void terminal_init(void);

#endif
