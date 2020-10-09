#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <elf.h>

enum {
	NOTYPE = 0, PLUS, MINUS, STAR, DIV,
	EQ, NOTEQ, OR, AND,
 	NOT, NEG, POINTER,
	LB, RB, HEX, DEC, REG, MARK
	//WARNING!! NOTEQ first and then NOT !!
	//WARNING!! HEX first and then DEC !!

	/* TODO: Add more token types */

};
const char *PRE = "044553312666000000";	//guess what?
static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				//spaces
	{"\\+", PLUS},					//plus
	{"-", MINUS},					//minus
	{"\\*", STAR},					//star
	{"/", DIV},						//div
	{"==", EQ},						//eq
	{"!=", NOTEQ},					//noteq
	{"&&", AND},					//and
	{"\\|\\|", OR},					//or
	{"!", NOT},						//not
	{"\\(", LB},					//lb
	{"\\)", RB},					//rb
	{"0[xX][0-9a-zA-Z]+", HEX},		//hex
	{"[0-9]+", DEC},				//dec
	{"\\$[a-z]+", REG},				//reg
	{"[a-zA-Z][A-Za-z0-9_]*", MARK},		//mark
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
	char str[64];
} Token;

Token tokens[64];
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
					case HEX:case DEC:case REG:case MARK:
						strncpy(tokens[nr_token].str, e + position - substr_len, substr_len);//regs or number
						tokens[nr_token].str[substr_len] = '\0';		//add '\0', it's very important
						//WARNING: 64 may be a little small...
					default:
						if(rules[i].token_type == MINUS) {	//solve neg
							if(nr_token == 0) tokens[nr_token++].type = NEG;
							else if(PLUS <= tokens[nr_token - 1].type && tokens[nr_token - 1].type <= LB) {
								tokens[nr_token++].type = NEG;
							} else tokens[nr_token++].type = MINUS;
						} else if(rules[i].token_type == STAR) { //solve pointer
							if(nr_token == 0) tokens[nr_token++].type = POINTER;
							else if(PLUS <= tokens[nr_token - 1].type && tokens[nr_token - 1].type <= LB) {
								tokens[nr_token++].type = POINTER;
							} else tokens[nr_token++].type = STAR;
						} else {
							tokens[nr_token++].type = rules[i].token_type;	//other	
						}
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
	int cnt = 0, flag = 1, i;		//A simple stack
	for(i = l;i <= r; i++){
		if(tokens[i].type == LB) ++cnt;
		if(tokens[i].type == RB) --cnt;
		if(cnt < 0)	return *success = false;//Bad
		if(i != r && cnt == 0) flag = 0;
	}
	if(cnt != 0) return *success = false;
	return flag;
}

uint32_t getAddressFromMark(char *mark, bool *success);

uint32_t eval(int l, int r, bool *success) {
	*success = true;
	if(l > r) return *success = false;// Bad Expression !!
	if(l == r){				//It's a number or reg, otherwise bad expression
		uint32_t tmp;
		if(tokens[l].type == HEX) {
			sscanf(tokens[l].str, "%x", &tmp);
			return tmp;
		} else if(tokens[l].type == DEC) {
			sscanf(tokens[l].str, "%d", &tmp);
			return tmp;
		} else if(tokens[l].type == REG) {	//read register
			const char *RE[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "eip"};
			const char *SE[] = {"es", "cs", "ss", "ds", "fs", "gs"};
			int i;
			for(i = 0; i < 9; i++) {
				if(strcmp(tokens[l].str + 1, RE[i]) == 0) return cpu.gpr[i]._32;
			}
			for(i = 0; i < 6; i++) {
				if(strcmp(tokens[l].str + 1, SE[i]) == 0) return cpu.sr[i].index;
			}
			if(strcmp(tokens[l].str + 1, "gdtr") == 0) return cpu.gdtr.base_addr;
			return *success = false; 
		} else if(tokens[l].type == MARK) {		//find mark
			return getAddressFromMark(tokens[l].str, success);
		}
		return *success = false;
	}
	bool flag = check_parentheses(l, r, success);
	if(!success) return 0;						//Bad
	if(flag) return eval(l + 1, r - 1, success);//OK, remove parentheses
	//Now we should find the dominant token
	int now = -1, type = 0x3f3f3f3f, cnt = 0, i;
	for(i = l; i <= r; i++) {
		if(tokens[i].type == LB) ++cnt;
		if(tokens[i].type == RB) --cnt;
		if(cnt != 0) continue;	//In mathched parentheses, pass
		if(PLUS <= tokens[i].type && tokens[i].type <= POINTER) {
			if(type >= PRE[tokens[i].type]) type = PRE[tokens[i].type], now = i;
		}
	}
	assert(now != -1);
	uint32_t a, b;
	//solve '!'
	if(tokens[now].type >= NOT) {
		//if type>=not, which means other token has been solved
		//so the first token must be NOT or NEG or POINTER
		b = eval(l + 1, r, success);
		if(!(*success)) return *success = false;
		if(tokens[l].type == NOT) return !b;
		if(tokens[l].type == NEG) return -b;
		if(tokens[l].type == POINTER) return swaddr_read(b, 1, R_DS);
		return *success = false;
	}
	a = eval(l, now - 1, success);
	if(!(*success))return *success = false;
	b = eval(now + 1, r ,success);
	if(!(*success))return *success = false;
	if(tokens[now].type == PLUS) return a + b;
	if(tokens[now].type == STAR) return a * b;
	if(tokens[now].type == DIV) return a / b;	
	if(tokens[now].type == MINUS) return a - b;
	if(tokens[now].type == EQ) return a == b;
	if(tokens[now].type == NOTEQ) return a != b;
	if(tokens[now].type == AND) return a && b;
	if(tokens[now].type == OR) return a || b;
	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	//Calculate the value
	return eval(0, nr_token - 1, success);//call eval to calculate the value of expression e
}
