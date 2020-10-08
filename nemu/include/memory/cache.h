#ifndef __CACHE_H__
#define __CACHE_H_

#define CACHE_BLOCK_SIZE_BIT 6
#define CACHE_WAY_BIT 3
#define CACHE_SET_BIT 7
#define CACHE_BLOCK_SIZE (1 << CACHE_BLOCK_SIZE_BIT)
#define CACHE_WAY_SIZE (1 << CACHE_WAY_BIT)
#define CACHE_SET_SIZE (1 << CACHE_SET_BIT)

typedef struct {
	uint8_t data[CACHE_BLOCK_SIZE];
	uint32_t tag;
	bool valid;
} CacheBlock;

CacheBlock cache[CACHE_SET_SIZE * CACHE_WAY_SIZE];

void resetCache();
int readCache(hwaddr_t addr);
void writeCache(hwaddr_t addr, size_t len, uint32_t data);

#endif
