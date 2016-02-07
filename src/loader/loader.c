#include <stdint.h>
#include <stdbool.h>

struct mm_entry {
	uint64_t base_addr;
	uint64_t addr_len;
	uint32_t type;
	uint32_t acpi_attrs;
};

enum mm_entry_type {
	MM_ENTRY_TYPE_CAN_USE		= 1,
	MM_ENTRY_TYPE_RESERVED		= 2,
};

#define BOOT_MM_ADDR	(void *)0x7e00

void loader_main(void)
{
	struct mm_entry *mm = (struct mm_entry *)BOOT_MM_ADDR;
	uint32_t cnt = *((uint32_t *)BOOT_MM_ADDR - 1);
	struct mm_entry mm_intervals[2*cnt];
	uint64_t intervals[2*cnt];
	uint32_t new_cnt = 0;

	// collect all bounds together
	for (uint32_t i = 0, j = 0; i < cnt; i++, j += 2) {
		intervals[j] = mm[i].base_addr;
		intervals[j+1] = mm[i].base_addr + mm[i].addr_len;
	}

	// sort bounds
	for (uint32_t i = 0; i < 2*cnt - 1; i++) {
		for (uint32_t j = i+1; j < 2*cnt; j++) {
			if (intervals[i] > intervals[j]) {
				uint64_t tmp = intervals[i];
				intervals[i] = intervals[j];
				intervals[j] = tmp;
			}
		}
	}

	// detect new interval types (now they are sorted and not overlaped)
	for (uint32_t i = 0; i < 2*cnt-1; i++) {
		bool exists = false;
		struct mm_entry e = {
			.base_addr = intervals[i],
			.addr_len = intervals[i+1] - intervals[i],
			.type = MM_ENTRY_TYPE_CAN_USE
		};

		if (e.addr_len == 0)
			// skip empty intervals
			continue;

		for (uint32_t j = 0; j < cnt; j++) {
			if (e.base_addr < mm[j].base_addr)
				continue;
			if (e.base_addr >= mm[j].base_addr + mm[j].addr_len)
				continue;

			exists = true;
			if (mm[j].type == MM_ENTRY_TYPE_RESERVED) {
				// In case at least one interval says that
				// this is reserved area -> this is reserved area
				e.type = MM_ENTRY_TYPE_RESERVED;
				break;
			}
		}

		if (exists == false)
			// skip non defined intervals
			continue;
		if (e.type != MM_ENTRY_TYPE_CAN_USE)
			continue;

		mm_intervals[new_cnt++] = e;
	}

	(void)mm_intervals;

	while (1)
		/*do nothing*/;
}
