#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_info(char *args){
	if(args == NULL) return 0;
	char opt;
	sscanf(args, " %c", &opt);
	if(opt == 'r') {
		printf("%%eax 0x%x\n", cpu.eax);
		printf("%%ecx 0x%x\n", cpu.ecx);
		printf("%%edx 0x%x\n", cpu.edx);
		printf("%%ebx 0x%x\n", cpu.ebx);
		printf("%%esp 0x%x\n", cpu.esp);
		printf("%%ebp 0x%x\n", cpu.ebp);
		printf("%%esi 0x%x\n", cpu.esi);
		printf("%%edi 0x%x\n", cpu.edi);
		printf("%%eip 0x%x\n", cpu.eip);
	}
	return 0;
}

static int cmd_si(char *args) {
	if(args == NULL) cpu_exec(1);
	else cpu_exec(atoi(args));
	return 0;
}

static int cmd_x(char *args) {
	if(args == NULL) return 0;
	uint32_t num, addr;
	sscanf(args, "%d%x", &num, &addr);
	while(num) {
		printf("address 0x%x: 0x%x\n", addr, swaddr_read(addr, 4));
		addr += 4;
		--num;
	}
	return 0;
}

static int cmd_p (char *args) {
	if(args == NULL) return 0;
	bool suc;
	uint32_t ans = expr(args, &suc);
	if(!suc) {
		printf("Bad expression\n");
		return 0;
	}
	printf("Expression %s : 0x%x\n", args, ans);
	return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);	//A pointer (Function)
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Continue the execution of the program in N steps", cmd_si },
	{ "info", "Print all registers", cmd_info },
	{ "x", "Scan the memory", cmd_x },
	{ "p", "Calculate the value of expression", cmd_p }
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
//		printf("%s\n",args);
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {	//Brute Force!
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
