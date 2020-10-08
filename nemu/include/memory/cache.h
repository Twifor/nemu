#ifndef __CACHE_H__
#define __CACHE_H_

#define CACHE_BLOCK_SIZE_BIT 6
#define CACHE_WAY_BIT 3
#define CACHE_SET_BIT 7
#define CACHE_BLOCK_SIZE (1 << CACHE_BLOCK_SIZE_BIT)
#define CACHE_WAY_SIZE (1 << CACHE_WAY_BIT)
#define CACHE_SET_SIZE (1 << CACHE_SET_BIT)

#define CACHE2_BLOCK_SIZE_BIT 6
#define CACHE2_WAY_BIT 4
#define CACHE2_SET_BIT 12
#define CACHE2_BLOCK_SIZE (1 << CACHE2_BLOCK_SIZE_BIT)
#define CACHE2_WAY_SIZE (1 << CACHE2_WAY_BIT)
#define CACHE2_SET_SIZE (1 << CACHE2_SET_BIT)

uint64_t MEMORY_TIME;

typedef struct {
	uint8_t data[CACHE_BLOCK_SIZE];
	uint32_t tag;
	bool valid;
} CacheBlock;

typedef struct {
	uint8_t data[CACHE2_BLOCK_SIZE];
	uint32_t tag;
	bool valid, dirty;
} CacheBlock2;

CacheBlock cache[CACHE_SET_SIZE * CACHE_WAY_SIZE];
CacheBlock2 cache2[CACHE2_SET_SIZE * CACHE2_WAY_SIZE];

void resetCache();
int readCache(hwaddr_t addr);
void writeCache(hwaddr_t addr, size_t len, uint32_t data);
void addMemoryTime(uint32_t t);
int readCache2(hwaddr_t addr);
void writeCache2(hwaddr_t addr, size_t len, uint32_t data);

#endif
