#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	long long ans = 1ll * a * b;
	return ans >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 *     asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */
	
	
	int sign = 1;
	if (a < 0) sign = -sign, a = -a;
	if (b < 0) sign = -sign, b = -b;
	int res = a / b;
	a = a % b;
	//you can't just return (a/b)<<16
	//which will lose last 16 bits
	int i;
	for (i = 0; i < 16; i++) 
	{
		a <<= 1;
		res <<= 1;
		if (a >= b) a -= b, ++res;
	}
	return res * sign;
}

FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */
	int b = *(int *)&a;//read float to int, you can't use (int)a
	int sign = b & 0x80000000;//sign
	int exp = (b >> 23) & 0xff;//exp
	int last = b & 0x7fffff;	//get last 23 bits
	if(exp == 255) return sign ? -0x7fffffff : 0x7fffffff;//inf, ignore NaN
	if(exp == 0) return 0;//0, ignore another one
	last |= 1 << 23;
	exp -= 134;	//guess why?
	if (exp < 0) last >>= -exp;
	if (exp > 0) last <<= exp;

	return sign ? -last : last;
}

FLOAT Fabs(FLOAT a) {
	return a < 0 ? -a : a;
}
//Debug Use
float toFloat(FLOAT s) {
	int sign = 1, i;
	if(s < 0)sign = -1,s = -s;
	float a = 0, as = 0.5;
	for(i = 15;i >= 0;i--) a += ((s & (1 << i)) >> i) * as, as /= 2;
	as = 1;
	for(i = 16;i < 32;i++) a += ((s & (1 << i)) >> i) * as,as *= 2;
	return sign * a;
}
/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

