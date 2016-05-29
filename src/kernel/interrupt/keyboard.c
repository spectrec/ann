#include <stdbool.h>

#include "stdlib/assert.h"

#include "kernel/lib/console/terminal.h"

#include "kernel/asm.h"
#include "kernel/task.h"
#include "kernel/monitor.h"
#include "kernel/interrupt/apic.h"
#include "kernel/interrupt/keyboard.h"

#define KEYBOARD_DATA_PORT	0x60
#define KEYBOARD_STATUS_PORT	0x64
#define KEYBOARD_COMMAND_PORT	0x64

#define KEYBOARD_KEY_RELEASED	0x80
void keyboard_handler(struct task *task)
{
	// XXX: check status is not needed, because interrupt will be
	// triggered only when data is ready.
	uint8_t scancode = inb(KEYBOARD_DATA_PORT);

	if ((scancode & KEYBOARD_KEY_RELEASED) != 0) {
		/* key relesed */
	} else {
		monitor_process_key_press(scancode);
	}

	APIC_WRITE(APIC_OFFSET_EOI, 0); // send EOI

	if (task->state == TASK_STATE_READY)
		task_run(task);

	schedule();
}
