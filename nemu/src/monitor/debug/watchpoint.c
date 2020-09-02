#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static int ID = 0;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

WP* new_wp() {
	assert(free_ != NULL);
	WP* ans = free_;
	free_ = ans->next;
	ans->next = NULL;
	return ans;
}

void wp_free(WP *wp) {
	wp->next = free_;
	free_ = wp;
}

WP* getHead() {
	return head;
}

int insertExpr(char *ex) {
	WP* nd = new_wp();
	nd->NO = ++ID;
	strcpy(nd->expr, ex);
	nd->next = head;
	head = nd;
	return ID;
}

int removeNode(int id) {
	if(head == NULL) return 0;
	if(head->NO == id){
		WP* tmp = head;
		head = head->next;
		wp_free(tmp);
		return 1;
	}
	WP* now = head;
	while(now->next != NULL) {
		if(now->next->NO == id) {
			WP* tmp = now->next;
			now->next = now->next->next;
			wp_free(tmp);
			return 1;
		}
		now = now->next;
	}
	return 0;
}

/* TODO: Implement the functionality of watchpoint */


