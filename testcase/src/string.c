#include "trap.h"
#include <string.h>

char *s[] = {
	//"ab",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	//"ab",
	", World!\n",
	"Hello, World!\n",
	"#####"
};

char str1[20] = "Hello";
char str[20];

int main() {
	//strcat(str, "H");
	nemu_assert(str[0]=='a');
	nemu_assert(strcmp(s[0], s[2]) == 0);
	nemu_assert(strcmp(s[0], s[1]) == -1);
	nemu_assert(strcmp(s[0] + 1, s[1] + 1) == -1);
	nemu_assert(strcmp(s[0] + 2, s[1] + 2) == -1);
	nemu_assert(strcmp(s[0] + 3, s[1] + 3) == -1);
	strcat(str1, s[3]);
	//set_bp();
	nemu_assert(strcmp(str1, s[4]) == 0);
	//strcpy(str, str1);
	//set_bp();
	//nemu_assert(strcmp(str, str1) == 0);
	//strcat(str, s[3]);
	//nemu_assert(strcmp(str, s[4]) == 0);

	nemu_assert(strcmp( strcat(strcpy(str, str1), s[3]), s[4]) == 0);

	nemu_assert(memcmp(memset(str, '#', 5), s[5], 5) == 0);

	return 0;
}
