#include "common.h"
#include "memory/cache.h"
#include "burst.h"
#include "cpu/reg.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	int first_id = readCache(addr);	//get cache id
	uint32_t offset = addr & (CACHE_BLOCK_SIZE - 1);
	uint8_t temp[2 * BURST_LEN];
	if(offset + len > CACHE_BLOCK_SIZE) {
		/* data cross the cache block boundary */
		int second_id = readCache(addr + CACHE_BLOCK_SIZE - offset);
		memcpy(temp, cache[first_id].data + offset, CACHE_BLOCK_SIZE - offset);
		memcpy(temp + CACHE_BLOCK_SIZE - offset, cache[second_id].data, len - CACHE_BLOCK_SIZE + offset);
	} else {
		memcpy(temp, cache[first_id].data + offset, len);
	}
	//hehe
	int zero = 0;
	uint32_t tmp = unalign_rw(temp + zero, 4) & (~0u >> ((4 - len) << 3));
	return tmp;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {	//physical address
	writeCache(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) { //linear address
	hwaddr_write(addr, len, data);
}

void loadSregCache(uint8_t sreg) {
	uint32_t gdt = cpu.gdtr.base_addr;//base
	gdt += cpu.sr[sreg].index << 3;//offset
	SegmentDescriptor sdp;
	sdp.first = lnaddr_read(gdt, 4);
	sdp.second = lnaddr_read(gdt + 4, 4);
	printf("%x %x %x\n",sdp.base1,sdp.base2,sdp.base3);
	uint32_t base = (((uint32_t)sdp.base2) << 16) | sdp.base1 | (((uint32_t)sdp.base3) << 24);
	uint32_t limit = (((uint32_t)sdp.limit2) << 16) | sdp.limit1;
	cpu.sr[sreg].cache.limit = limit;
	cpu.sr[sreg].cache.base = base;
}

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg) {
	if(cpu.PE) {//protected mode
		return addr + cpu.sr[sreg].cache.base;
	} else {
		return addr;//real mode
	}
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {	//virtual address
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}
