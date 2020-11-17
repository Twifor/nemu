#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PT_SIZE - 1) / PT_SIZE)

/* Use the function to get the start address of user page directory. */
PDE* get_updir();
static PTE vmem_pt[1024] align_to_page;

void create_video_mapping() {
	//000a 0000
	/* TODO: create an identical mapping from virtual memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	PDE *pdp = (PDE *)va_to_pa(get_updir());
	pdp[0].page_frame = (uint32_t) va_to_pa(vmem_pt) >> 12;
	pdp[0].present = 1;
	int i;
	for(i = VMEM_ADDR / PAGE_SIZE; i < (VMEM_ADDR + SCR_SIZE) / PAGE_SIZE + 1; i++) {
		vmem_pt[i].page_frame = i;
		vmem_pt[i].present = 1;
	}
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

