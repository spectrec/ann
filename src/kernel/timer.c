#include "kernel/apic.h"
#include "kernel/task.h"
#include "kernel/timer.h"

int timer_init(void)
{
	// TODO: initialize timer
	//APIC_WRITE(APIC_OFFSET_LVT_TIMER, (1ul << 17) | 0x20);
	//APIC_WRITE(APIC_OFFSET_DCR, 0b1001);
	//APIC_WRITE(APIC_OFFSET_ICR, 2083333);
	//APIC_WRITE(APIC_OFFSET_SVR, 0xff | APIC_SVR_ENABLE);

	return 0;
}

void timer_handler(struct task *task)
{
	APIC_WRITE(APIC_OFFSET_EOI, 0); // send EOI

	task_run(task);
}
