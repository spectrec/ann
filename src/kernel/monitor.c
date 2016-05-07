#include <stddef.h>

#include "stdlib/assert.h"
#include "stdlib/string.h"
#include "stdlib/stdlib.h"

#include "kernel/task.h"
#include "kernel/monitor.h"

#include "kernel/lib/console/terminal.h"

#define COMMAND_LINE_PROMPT "-> "
#define COMMAND_LINE_PROMPT_LEN (sizeof(COMMAND_LINE_PROMPT) - 1)

static struct terminal_position command_line_position;

enum special_key {
	KEY_ESCAPE	= 0x1,
	KEY_BACKSPACE	= 0xE,
	KEY_LEFT_CTRL	= 0x1d,
	KEY_LEFT_SHIFT	= 0x2a,
	KEY_RIGHT_SHIFT	= 0x36,
	KEY_LEFT_ALT	= 0x38,
	KEY_CAPSLOCK	= 0x3a,
};

static const char scancodes[128] = {
	0, 0 /* escape */, '1', '2', '3', '4', '5',
	'6', '7', '8', '9', '0', '-', '=',
	0 /* backspace */, '\t', 'q', 'w', 'e', 'r',
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0 /*left control*/, 'a', 's', 'd', 'f', 'g',
	'h', 'j', 'k', 'l', ';', '\'', '`', 0 /*left shift*/,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.',
	'/', 0 /*right shift*/, '*', 0 /*left alt*/, ' ',
	0 /*caps lock*/, 0 /*f1*/, 0 /*f2*/, 0 /*f3*/, 0 /*f4*/,
	0 /*f5*/, 0 /*f6*/, 0 /*f7*/, 0 /*f8*/, 0 /*f9*/, 0 /*f10*/,
	0 /*num lock*/, 0 /*scroll lock*/
};

void monitor_init(void)
{
	command_line_position.row = TERMINAL_ROW_COUNT;
	command_line_position.column = 0;

	struct terminal_position p = terminal_position();
	terminal_set_position(command_line_position);

	terminal_printf("%s", COMMAND_LINE_PROMPT);
	command_line_position = terminal_position();

	terminal_set_position(p);
}

static void monitor_process_command(const char *command);
void monitor_process_key_press(uint8_t scancode)
{
	struct terminal_position p = terminal_position();
	uint8_t code = scancodes[scancode];
	const char *command = NULL;

	terminal_set_position(command_line_position);
	if (scancode == KEY_BACKSPACE &&
	    command_line_position.column > COMMAND_LINE_PROMPT_LEN) {
		command_line_position.column--;
		terminal_set_position(command_line_position);
		terminal_put(' ');
	} else if (code == '\n') {
		command = terminal_read_command(COMMAND_LINE_PROMPT_LEN);
		assert(command != NULL);

		terminal_clear_line();
	} else if (code != 0) {
		if (command_line_position.column < TERMINAL_COL_COUNT-1) {
			command_line_position.column++;
			terminal_put(code);
		}
	}

	if (command != NULL) {
		// redraw command line prompt
		terminal_printf("%s", COMMAND_LINE_PROMPT);
		command_line_position = terminal_position();
	}

	terminal_set_position(p);
	if (command != NULL)
		monitor_process_command(command);
}

static void help_command_handler(int argc, char *argv[]);
static void clear_command_handler(int argc, char *argv[]);

static void ps_command_handler(int argc, char *argv[]);
static void kill_command_handler(int argc, char *argv[]);

typedef void (*command_handler_t)(int argc, char *argv[]);
static const struct monitor_command {
	const char *name;
	const char *description;
	command_handler_t handler;
} monitor_command[] = {
	// common
	{ .name = "clear",	.description = "clear screen",			.handler = clear_command_handler },
	{ .name = "help",	.description = "show this message",		.handler = help_command_handler },

	// process related
	{ .name = "ps",		.description = "show running processes",	.handler = ps_command_handler },
	{ .name = "kill",	.description = "kill process by id",		.handler = kill_command_handler },

	{ .name = "",		.description = "end of commands list",		.handler = NULL },
};

static void monitor_process_command(const char *c)
{
	char *argv[128] = { NULL };
	char command[256];
	int argc = 0;

	strncpy(command, c, sizeof(command));
	for (char *p = command; *p != '\0'; p++) {
		if (argv[argc] == NULL) {
			if (*p == ' ')
				continue;

			argv[argc] = p;
		} else {
			if (*p != ' ')
				continue;

			*p = '\0';
			argc++;
		}
	}

	if (argv[argc] != NULL)
		argc++;

	for (int i = 0; argc > 0 && monitor_command[i].handler != NULL ; i++) {
		if (strcmp(argv[0], monitor_command[i].name) != 0)
			continue;

		return monitor_command[i].handler(argc, argv);
	}

	terminal_printf("unknown command: `%s'\n", argv[0] ?: "");
	help_command_handler(0, NULL);
}

static void help_command_handler(int argc, char *argv[])
{
	(void)argc; (void)argv;

	terminal_printf("Available commands:\n");
	for (int i = 0; monitor_command[i].handler != NULL; i++) {
		terminal_printf("`%s': %s\n", monitor_command[i].name,
					      monitor_command[i].description);
	}
}

static void clear_command_handler(int argc, char *argv[])
{
	(void)argc; (void)argv;

	terminal_clear();
	monitor_init();
}

static void ps_command_handler(int argc, char *argv[])
{
	(void)argc; (void)argv;

	task_list();
}

static void kill_command_handler(int argc, char *argv[])
{
	if (argc != 2)
		return terminal_printf("Usage: kill <task_id>\n");

	task_kill(atoi(argv[1]));
}
