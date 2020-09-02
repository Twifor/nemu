#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

const char *DEBUG_P = " +-*/()HDR";
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

	{" +",	NOTYPE},				//spaces
	{"\\+", PLUS},					//plus
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
					case HEX:case DEC:case REG:
						strncpy(tokens[nr_token].str, e + position - substr_len, substr_len);//regs or number
						tokens[nr_token].str[substr_len] = '\0';
						//WARNING: 64 may be a little small...
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

int findRightBorder(int l, int r) {
	if(l > r) return -1;
	int cnt = 0; //stack
	for(int i = l; i <= r; i++) {
		if(tokens[i].type == LB) ++cnt;
		if(tokens[i].type == RB) --cnt;
		if(tokens[i].type != RB && tokens[i].type != DEC && tokens[i].type != HEX && tokens[i].type != REG) continue;
		if(cnt == 0) return i;
	}
	return -1;
}

void insertToken(int at) {
	for(int i = nr_token - 1; i >= at; i--) tokens[i + 1] = tokens[i];
	++nr_token;
}

int findNeg(int l, int r) {
	for(int i = l; i <= r ;i++) {
		if(tokens[i].type != MINUS) continue;
		if(i == l) return i;
		switch(tokens[i - 1].type) {
			case PLUS:case STAR:case MINUS:case DIV:
				return i;
		}
	}
	return -1;
}

uint32_t eval(int l, int r, bool *success) {
	*success = true;
	if(l > r) return *success = false;// Bad Expression !!
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
		return *success = false;
	}
	bool flag = check_parentheses(l, r, success);
	if(!success) return 0;						//Bad
	if(flag) return eval(l + 1, r - 1, success);//OK, remove parentheses
	//Now we should find the dominant token
	int now = -1, type = -1, cnt = 0;
	for(int i = l; i <= r; i++) {
		if(tokens[i].type == LB) ++cnt;
		if(tokens[i].type == RB) --cnt;
		if(cnt != 0) continue;	//In mathched parentheses, pass
		if(tokens[i].type == PLUS || tokens[i].type == MINUS) {	//+ or -
			type = tokens[i].type, now = i;
		} else if(tokens[i].type == STAR || tokens[i].type == DIV) {	//* or /
			if(type == -1 || type == STAR || type == DIV) type = tokens[i].type, now = i;
		}
	}
	assert(now != -1);
	uint32_t a, b;
	a = eval(l, now - 1, success);
	if(!(*success))return *success = false;
	b = eval(now + 1, r ,success);
	if(!(*success))return *success = false;
	if(tokens[now].type == PLUS) return a + b;
	if(tokens[now].type == STAR) return a * b;
	if(tokens[now].type == DIV) return a / b;	
	if(tokens[now].type == MINUS) return a - b;
	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	//for(int i = 0;i<nr_token;i++){
	//	if(tokens[i].type < HEX)printf("%c",DEBUG_P[tokens[i].type]);
	//	else printf("%s", tokens[i].str);
	//}	
	//printf("\n");
	//solve '-'
	int at;
	while((at = findNeg(0, nr_token - 1)) != -1) {
		int border = findRightBorder(at + 1, nr_token - 1);
		if(border == -1) return *success = false;
		insertToken(border + 1), tokens[border + 1].type = RB;
		insertToken(at), tokens[at].type = DEC, strcpy(tokens[at].str, "0");
		insertToken(at), tokens[at].type = LB;
	}
	//for(int i = 0;i<nr_token;i++){
	//	if(tokens[i].type < HEX)printf("%c",DEBUG_P[tokens[i].type]);
	//	else printf("%s", tokens[i].str);
	//}	
	//printf("\n");
	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	//Calculate the value
	return eval(0, nr_token - 1, success);//call eval to calculate the value of expression e
}

