#ifndef __MONITOR_H__
#define __MONITOR_H__

#include <stdint.h>

void monitor_init(void);
void monitor_process_key_press(uint8_t scancode);

#endif
