#include "common.h"
#include <stdlib.h>
#include "burst.h"

#define CACHE_BLOCK_SIZE_BIT 6
#define CACHE_WAY_BIT 3
#define CACHE_SET_BIT 7
#define CACHE_BLOCK_SIZE (1 << CACHE_BLOCK_SIZE_BIT)
#define CACHE_WAY_SIZE (1 << CACHE_WAY_BIT)
#define CACHE_SET_SIZE (1 << CACHE_SET_BIT)

void ddr3_read_public(hwaddr_t addr, void *data);

typedef struct {
	uint8_t data[CACHE_BLOCK_SIZE];
	uint32_t tag;
	bool valid;
} CacheBlock;

CacheBlock cache[CACHE_SET_SIZE * CACHE_WAY_SIZE];


void resetCache() {
	int i = 0;
	for(i = 0; i < CACHE_SET_SIZE * CACHE_WAY_SIZE; i++) cache[i].valid = false;
}

//return ID
int readCache(hwaddr_t addr) {
	uint32_t tag = addr >> (CACHE_BLOCK_SIZE_BIT + CACHE_SET_BIT);
	uint32_t set = (addr >> CACHE_BLOCK_SIZE_BIT) & (CACHE_SET_SIZE - 1);
	uint32_t block = (addr >> CACHE_BLOCK_SIZE_BIT) << CACHE_BLOCK_SIZE_BIT;
	//not offset
	int i = 0, j;
	for(i = CACHE_WAY_SIZE * set; i < CACHE_WAY_SIZE * (set + 1); i++) {
		if(cache[i].tag == tag && cache[i].valid) return i;
	}
	srand(i);
	i = CACHE_WAY_SIZE * set + rand() % CACHE_WAY_SIZE;//random
	for(j = 0; j < CACHE_BLOCK_SIZE / BURST_LEN; j++) {
		ddr3_read_public(block + j * BURST_LEN , cache[i].data + j * BURST_LEN);
	}
	cache[i].valid = true;
	cache[i].tag = tag;
	return i;
}
