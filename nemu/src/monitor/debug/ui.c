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
	} else if(opt == 'w') {
		WP *h = getHead();
		while(h != NULL) {
			printf("watchpoint %d : %s\n", h->NO, h->expr);
			h = h->next;
		}
	} else if(opt == 's') {
		printf("%%gdtr base:0x%x limit:0x%x\n", cpu.gdtr.base_addr, cpu.gdtr.seg_limit);
		const char *S[] = {"es", "cs", "ss", "ds", "fs", "gs"};
		int i;
		for(i = 0; i < 6; i++) {
			printf("%%%s 0x%x base: 0x%x limit: 0x%x\n", S[i], cpu.sr[i].index, cpu.sr[i].cache.base, cpu.sr[i].cache.limit);
		}
	} else if(opt == 'c') {
		printf("%%cr0 0x%x\n", cpu.cr0.val);
		printf("%%cr3 0x%x 0x%x\n", cpu.cr3.val, cpu.cr3.page_directory_base);
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
	uint32_t num = 0, addr;
	bool suc;
	while(args[0] == ' ')++args;	//trim
	while('0' <= args[0] && args[0] <= '9') num = (num << 3) + (num << 1) + (args[0] & 15), ++args;
	//get number
	addr = expr(args, &suc);
	if(!suc) {
		printf("\033[1;31mInvalid expression\n\033[0m");
		return 0;
	}
	while(num) {
		printf("address 0x%x:", addr);
		int i;
		for(i = 0;i < 4; i++)printf(" 0x%x", swaddr_read(addr + i, 1, R_DS));
		printf("\n");
		addr += 4;
		--num;
	}
	return 0;
}

static int cmd_p(char *args) {
	if(args == NULL) return 0;
	bool suc;
	uint32_t ans = expr(args, &suc);	//fix bugs
	if(!suc) {
		printf("\033[1;31mInvalid expression\n\033[0m");
		return 0;
	}
	//tokens;
	printf("Expression %s : 0x%x\n", args, ans);
	return 0;
}

static int cmd_w(char *args) {
	if(args == NULL) return 0;
	int id = insertExpr(args);
	if(id == -1) {
		printf("\033[1;31mInvalid expression\n\033[0m");
		return 0;
	}
	printf("Add watchpoint %d\n", id);
	return 0;
}

static int cmd_d(char *args) {
	if(args == NULL) return 0;
	int id;
	sscanf(args, "%d", &id);
	int ans = removeNode(id);//remove a node
	if(ans == 0) {
		printf("\033[1;31mWatchpoint %d doesn't exist\n\033[0m", id);
	} else {
		printf("Delete watchpoint %d successfully\n", id);
	}
	return 0;
}

static int cmd_goto(char *args) {
	if(args == NULL) return 0;
	bool suc;
	uint32_t ans = expr(args, &suc);
	if(!suc) {
		printf("\033[1;31mInvalid expression\n\033[0m");
		return 0;
	}
	cpu.eip = ans;
	printf("Goto address 0x%x successfully\n", ans);
	return 0;
}

void getFunctionFromAddress(swaddr_t addr, char *s);

static int cmd_bt(char *args) {
	swaddr_t now_ebp = reg_l(R_EBP);
	swaddr_t now_ret = cpu.eip;
	int cnt = 0, i;
	char name[50];
	while(now_ebp) {
		getFunctionFromAddress(now_ret, name);
		if(name[0] == '\0') break;
		printf("#%d 0x%x: ", ++cnt, now_ret);
		printf("%s (", name);
		for(i = 0; i < 4; i++) {
			printf("%d", swaddr_read(now_ebp + 8 + i * 4, 4, R_SS));
			printf("%c", i == 3 ? ')' : ',');
		}
		now_ret = swaddr_read(now_ebp + 4, 4, R_SS);
		now_ebp = swaddr_read(now_ebp, 4, R_SS);
		printf("\n");
	}
	return 0;
}

static int cmd_page(char *args) {
	if(args == NULL) return 0;
	lnaddr_t lnaddr;
	sscanf(args, "%x", &lnaddr);
	hwaddr_t hwaddr = page_translate(lnaddr, 1);
	if(!cpu.cr0.protect_enable || !cpu.cr0.paging) {
		printf("\033[1;33mPage address convertion is invalid.\n\033[0m");
	}
	printf("0x%x -> 0x%x\n", lnaddr, hwaddr);
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
	{ "p", "Calculate the value of expression", cmd_p },
	{ "w", "Add a watchpoint", cmd_w },
	{ "d", "Delete a watchpoint", cmd_d },
	{ "goto", "Goto address", cmd_goto },
	{ "bt", "Print backtrace", cmd_bt },
	{ "page", "Convert virtual address to physical address", cmd_page },
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
