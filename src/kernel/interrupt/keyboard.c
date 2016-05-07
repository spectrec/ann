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

// must be set before attempting to read from data port
#define KEYBOARD_STATUS_FLAG_OUT_BUFFER_FULL	(1 << 0)
// must be clear before attempting to write to data or command port
#define KEYBOARD_STATUS_FLAG_IN_BUFFER_FULL	(1 << 1)
#define KEYBOARD_STATUS_SYSTEM_FLAG		(1 << 2)
// data written to input buffer is command
#define KEYBOARD_STATUS_COMMAND_FLAG		(1 << 3)
// chipset specific				(1 << 4)
// chipset specific				(1 << 5)
#define KEYBOARD_STATUS_TIMEOUT_ERORR		(1 << 6)
#define KEYBOARD_STATUS_PARITY_ERORR		(1 << 7)

static int keyboard_wait(bool write, bool wait)
{
	do {
		uint8_t status = inb(KEYBOARD_STATUS_PORT);

		if ((status & KEYBOARD_STATUS_TIMEOUT_ERORR) != 0) {
			terminal_printf("keyboard timeout error");
			return -1;
		}
		if ((status & KEYBOARD_STATUS_PARITY_ERORR) != 0) {
			terminal_printf("keyboard parity error");
			return -1;
		}

		if (write == true && (status & KEYBOARD_STATUS_FLAG_IN_BUFFER_FULL) == 0)
			// we can write only when buffer is empty
			return 0;
		if (write == false && (status & KEYBOARD_STATUS_FLAG_OUT_BUFFER_FULL) != 0)
			// we can read only when buffer is full
			return 0;
	} while (wait == true);

	// controller is not ready
	return 1;
}

#define KEYBOARD_READ_OPERATION		0x0
#define KEYBOARD_WRITE_OPERATION	0x1
int keyboard_init(void)
{
	uint8_t data;
	int r;

	// First of all: we must disable devices
	if (keyboard_wait(KEYBOARD_WRITE_OPERATION, true) != 0)
		return -1;
	outb(KEYBOARD_COMMAND_PORT, 0xAD);
	if (keyboard_wait(KEYBOARD_WRITE_OPERATION, true) != 0)
		return -1;
	outb(KEYBOARD_COMMAND_PORT, 0xA7);

	// Now we must flush keyboard buffer
	while ((r = keyboard_wait(KEYBOARD_READ_OPERATION, false)) != 1) {
		if (r == -1)
			// some error occurred
			return -1;

		// skip input, we want just flush the buffer
		inb(KEYBOARD_DATA_PORT);
	}

	// Perform self test
	if (keyboard_wait(KEYBOARD_WRITE_OPERATION, true) != 0)
		return -1;
	outb(KEYBOARD_COMMAND_PORT, 0xAA);
	if (keyboard_wait(KEYBOARD_READ_OPERATION, true) != 0)
		return -1;
	if ((data = inb(KEYBOARD_DATA_PORT)) != 0x55) {
		terminal_printf("Invalid response from PS/2 controller: %x\n", data);
		return -1;
	}

	// Test the first PS/2 port
	if (keyboard_wait(KEYBOARD_WRITE_OPERATION, true) != 0)
		return -1;
	outb(KEYBOARD_COMMAND_PORT, 0xAB);
	if (keyboard_wait(KEYBOARD_READ_OPERATION, true) != 0)
		return -1;
	if ((data = inb(KEYBOARD_DATA_PORT)) != 0x0) {
		terminal_printf("Test the first PS/2 port failed, ret: %x\n", data);
		return -1;
	}

	// Enable the first PS/2 port
	if (keyboard_wait(KEYBOARD_WRITE_OPERATION, true) != 0)
		return -1;
	outb(KEYBOARD_COMMAND_PORT, 0xAE);

	return 0;
}

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
