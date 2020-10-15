#include "memory/tlb.h"

uint32_t readTLB(uint32_t tg) {
	int i;
	for(i = 0; i < TLB_SIZE; i++) {
		if(tlb[i].valid && tlb[i].tag == tg) return tlb[i].page;
	}
	return -1;//invalid
}

void resetTLB() {
	int i;
	for(i = 0; i < TLB_SIZE; i++) {
		tlb[i].valid = false;
	}
}

void writeTLB(uint32_t tg, uint32_t page) {
	int i;
	for(i = 0; i < TLB_SIZE; i++) {
		if(!tlb[i].valid) {
			tlb[i].valid = true;
			tlb[i].tag = tg;
			tlb[i].page = page;
			return;
		}
	}
	i = rand() % TLB_SIZE;
	tlb[i].valid = true;
	tlb[i].tag = tg;
	tlb[i].page = page;
	return;
}
