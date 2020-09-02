#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char expr[64];
	uint32_t ans;
	/* TODO: Add more members if necessary */

} WP;

WP* new_wp();

void wp_free(WP *wp);

WP* getHead();

int insertExpr(char *ex);

int removeNode(int id);

int checkNode(WP *nd);
#endif
