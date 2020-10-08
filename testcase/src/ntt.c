/*
* A program to test NEMU.
*/
#include "trap.h"

#define N 20
#define MOD 998244353
#define G 3
int tr[N << 2];

typedef struct {
    int l, op[N << 2];
} Pol;
Pol a, b;

int qMul(int a, int b)
{
	if(b < 0) b = (b % MOD + MOD) % MOD;
    int ans = 0;
    while(b) {
        if(b & 1) ans = (ans % MOD + a % MOD) % MOD;
		a = (a % MOD + a % MOD) % MOD;
		b >>= 1;
    }
    return ans % MOD;
}

int qPow(int a, int b) {
    int ans = 1, x = a;
    while (b) {
        if (b & 1) ans = qMul(ans, x);
        x = qMul(x, x), b >>= 1;
    }
    return ans;
}

void swap(int *a, int *b) {
	int t = *a;
	*a = *b;
	*b = t;
}

void NTT(int l, int *c, int type) {
	int i, mid, j, len, k;
    for (i = 0; i < l; i++)if (i < tr[i])swap(&c[i], &c[tr[i]]);
    for (mid = 1; mid < l; mid <<= 1) {
        int wn = qPow(G, (MOD - 1) / (mid << 1));
        if (type == -1)wn = qPow(wn, MOD - 2);
        for (len = mid << 1, j = 0; j < l; j += len) {
            int w = 1;
            for (k = 0; k < mid; k++, w = qMul(w, wn)) {
                int x = c[j + k], y = qMul(w, c[j + mid + k]);
                c[j + k] = (x + y) % MOD, c[j + mid + k] = (x - y) % MOD;
            }
        }
    }
}

void multiple(Pol *ans, Pol *a, Pol *b) {
    int l = 1, le = 0, i;
    while (l <= a->l + b->l)l <<= 1, ++le;
    for (i = 0; i < l; i++) {
        tr[i] = (tr[i >> 1] >> 1) | ((i & 1) << (le - 1));
        if (i > a->l)a->op[i] = 0;
        if (i > b->l)b->op[i] = 0;
    }
   	NTT(l, a->op, 1);
	NTT(l, b->op, 1);
    for (i = 0; i < l; i++)a->op[i] = qMul(a->op[i], b->op[i]);
    NTT(l, a->op, -1);
	l = qPow(l, MOD - 2), ans->l = a->l + b->l;
    for (i = 0; i <= ans->l; i++)ans->op[i] = qMul(ans->op[i], l);
}

int main() {
	int i;
	a.l = b.l = 10;
	for(i = 0; i <= 10; i++) a.op[i] = b.op[i] = 1;
	multiple(&a, &a, &b);
	nemu_assert(a.l == 20);
	for(i = 0; i <= 10; i++) a.op[i] -= i;
	for(i = 11; i <= 20; i++) a.op[i] += i;
	for(i = 0;i <= a.l; i++) a.op[i] = (a.op[i] + MOD) % MOD;
	for(i = 0; i <= 10; i++) nemu_assert(a.op[i] == 1);
	for(i = 11; i<= 20; i++) nemu_assert(a.op[i] == 21);
	return 0;
}
