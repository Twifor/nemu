#include "common.h"
#include "memory/cache.h"
#include <stdlib.h>
#include "burst.h"

void ddr3_read_public(hwaddr_t addr, void *data);

void addMemoryTime(uint32_t t) {
	MEMORY_TIME += t;
}

void resetCache() {
	int i = 0;
	MEMORY_TIME = 0;//debug use
	for(i = 0; i < CACHE_SET_SIZE * CACHE_WAY_SIZE; i++) cache[i].valid = false;
	for(i = 0; i < CACHE2_SET_SIZE * CACHE2_WAY_SIZE; i++) cache2[i].valid = false;
}

//return ID
int readCache(hwaddr_t addr) {
	uint32_t tag = addr >> (CACHE_BLOCK_SIZE_BIT + CACHE_SET_BIT);
	uint32_t set = (addr >> CACHE_BLOCK_SIZE_BIT) & (CACHE_SET_SIZE - 1);
	//not offset
	int i = 0, j;
	for(i = CACHE_WAY_SIZE * set; i < CACHE_WAY_SIZE * (set + 1); i++) {
		if(cache[i].tag == tag && cache[i].valid) return i;
	}
	j = readCache2(addr);
	srand(i);
	i = CACHE_WAY_SIZE * set + rand() % CACHE_WAY_SIZE;//random
	memcpy(cache[i].data, cache2[j].data, CACHE_BLOCK_SIZE);//CACHE2_BLOCK_SIZE=CACHE_BLOCK_SIZE
	cache[i].valid = true;
	cache[i].tag = tag;
	return i;
}

int readCache2(hwaddr_t addr) {
	uint32_t tag = addr >> (CACHE2_BLOCK_SIZE_BIT + CACHE2_SET_BIT);
	uint32_t set = (addr >> CACHE2_BLOCK_SIZE_BIT) & (CACHE2_SET_SIZE - 1);
	uint32_t block = (addr >> CACHE2_BLOCK_SIZE_BIT) << CACHE2_BLOCK_SIZE_BIT;
	//not offset
	int i = 0, j;
	for(i = CACHE2_WAY_SIZE * set; i < CACHE2_WAY_SIZE * (set + 1); i++) {
		if(cache2[i].tag == tag && cache2[i].valid) return i;
	}
	srand(i);
	i = CACHE2_WAY_SIZE * set + rand() % CACHE2_WAY_SIZE;//random
	for(j = 0; j < CACHE2_BLOCK_SIZE / BURST_LEN; j++) {
		ddr3_read_public(block + j * BURST_LEN , cache2[i].data + j * BURST_LEN);
	}
	cache2[i].valid = true;
	cache2[i].tag = tag;
	cache2[i].dirty = false;
	printf("x");
	return i;
}

void writeCache(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t tag = addr >> (CACHE_BLOCK_SIZE_BIT + CACHE_SET_BIT);
	uint32_t set = (addr >> CACHE_BLOCK_SIZE_BIT) & (CACHE_SET_SIZE - 1);
	uint32_t offset = addr & (CACHE_BLOCK_SIZE - 1);
	//not offset
	int i = 0;
	for(i = CACHE_WAY_SIZE * set; i < CACHE_WAY_SIZE * (set + 1); i++) {
		if(cache[i].tag == tag && cache[i].valid) {
			if(offset + len > CACHE_BLOCK_SIZE) {//across
				memcpy(cache[i].data + offset, &data, CACHE_BLOCK_SIZE - offset);
				writeCache(addr + CACHE_BLOCK_SIZE - offset, len - CACHE_BLOCK_SIZE + offset, data >> (CACHE_BLOCK_SIZE - offset));
			} else {
				memcpy(cache[i].data + offset, &data, len);
			}
			return;
		}
	}
	
}
