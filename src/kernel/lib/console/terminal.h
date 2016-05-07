#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#ifdef __USER__
# error "This file is for kernel internal use only"
#endif

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

struct terminal_position {
	uint8_t row;
	uint8_t column;
};

uint8_t terminal_make_color(enum terminal_color fg, enum terminal_color bg);

void terminal_put_color(uint8_t ch, uint8_t color);
void terminal_vprintf(const char *fmt, va_list ap);
void terminal_printf(const char *fmt, ...);
void terminal_put(uint8_t ch);

struct terminal_position terminal_position(void);
void terminal_set_position(struct terminal_position p);

void terminal_clear(void);
void terminal_init(void);

const char *terminal_read_command(uint8_t off);
void terminal_clear_line(void);

#define TERMINAL_ROW_COUNT	24
#define TERMINAL_COL_COUNT	80

#endif
