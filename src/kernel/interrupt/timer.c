#include "kernel/lib/console/terminal.h"

#include "kernel/task.h"
#include "kernel/interrupt/apic.h"
#include "kernel/interrupt/timer.h"
#include "kernel/interrupt/interrupt.h"

#define TIMER_INITIAL_COUNT	60000000
#define TIMER_PERIODIC		(1 << 17)

int timer_init(void)
{
	APIC_WRITE(APIC_OFFSET_ICR, TIMER_INITIAL_COUNT);
	APIC_WRITE(APIC_OFFSET_DCR, APIC_DCR_NODIV);
	APIC_WRITE(APIC_OFFSET_LVT_TIMER, TIMER_PERIODIC | INTERRUPT_VECTOR_TIMER);

	return 0;
}

void timer_handler(struct task *task)
{
	(void)task;

	APIC_WRITE(APIC_OFFSET_EOI, 0); // send EOI

	schedule();
}
