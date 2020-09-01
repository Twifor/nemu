#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, PLUS, MINUS, STAR, DIV, LB, RB, HEX, DEC, REG

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", PLUS},					// plus
	{"-", MINUS},					//minus
	{"\\*", STAR},					//star
	{"/", DIV},						//div
	{"\\(", LB},					//lb
	{"\\)", RB},					//rb
	{"0[xX][0-9a-zA-Z]+", HEX},		//hex
	{"[0-9]+", DEC},				//dec
	{"\\$[a-z]+", REG}				//reg
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				//char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;
				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE:
						break;											//It's blank!
					case HEX:case DEC:case REG:
						strncpy(tokens[nr_token].str, e + position - substr_len, substr_len);//regs or number
						//WARNING: 32 may be a little small...
					default:
						tokens[nr_token++].type = rules[i].token_type;	//other	
						break;
					//panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int l, int r, bool *success) {//Check the parentheses, use stack.
	*success = true;
	if(l > r) return *success = false;
	int cnt = 0, flag = 1;		//A simple stack
	for(int i = l;i <= r; i++){
		if(tokens[i].type == LB) ++cnt;
		if(tokens[i].type == RB) --cnt;
		if(cnt < 0)	return *success = false;//Bad
		if(i != r && cnt == 0) flag = 0;
	}
	if(cnt != 0) return *success = false;
	return flag;
}

uint32_t eval(int l, int r, bool *success) {
	*success = true;
	if(l > r) {
		*success = false;	//Bad Expression !!
		return 0;
	}
	if(l == r){				//It's a number or reg, otherwise bad expression
		uint32_t tmp;
		if(tokens[l].type == HEX) {
			sscanf(tokens[l].str, "%x", &tmp);
			return tmp;
		}else if(tokens[l].type == DEC) {
			sscanf(tokens[l].str, "%d", &tmp);
			return tmp;
		}else if(tokens[l].type == REG) {
			return 0;			//Pass....
		}
		*success = false;
		return 0;
	}
	bool flag = check_parentheses(l, r, success);
	if(!success) return 0;						//Bad
	if(flag) return eval(l + 1, r - 1, success);//OK
	//Now we should find the dominant token!
	int now = -1, type = -1, cnt = 0;
	for(int i = l; i <= r; i++) {
		if(tokens[i].type == LB) ++cnt;
		if(tokens[i].type == RB) --cnt;
		if(cnt != 0) continue;	//In parentheses, pass
		if(tokens[i].type == PLUS || tokens[i].type == MINUS) {	//+ or -
			type = tokens[i].type, now = i;
		} else if(tokens[i].type == STAR || tokens[i].type == DIV) {	//* or /
			if(type == -1 || type == STAR || type == DIV) type = tokens[i].type, now = i;
		}
	}
	assert(now != -1);
	//solve "-"
	uint32_t a, b;
	if(tokens[now].type == MINUS) {	//It's "-"
		if(now == l) return -eval(now + 1, r, success);//Just -1
		switch (tokens[now - 1].type) {
			case PLUS:case MINUS:case STAR:case DIV://other token
				a = eval(l, now - 2, success);
				if(!(*success)) return *success = false;
				b = -eval(now + 1, r, success);
				if(!(*success)) return *success = false;
				--now;
				break;
			default:
				a = eval(l, now - 1, success);
				if(!(*success)) return *success = false;
				b = eval(now + 1, r, success);
				if(!(*success)) return *success = false;
				return a - b;
		}
	} else {
		a = eval(l, now - 1, success);
		if(!(*success))return *success = false;
		b = eval(now + 1, r ,success);
		if(!(*success))return *success = false;
	}
	if(tokens[now].type == PLUS) return a + b;
	if(tokens[now].type == STAR) return a * b;
	if(tokens[now].type == DIV) return a / b;	
	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	
	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	//It's may be a little difficult...
	//Calculate the value!
	return eval(0, nr_token - 1, success);//call eval to calculate the value of expression e
}

