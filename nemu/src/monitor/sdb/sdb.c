/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/vaddr.h>
#include <stdbool.h>
#include <stdint.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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

static int cmd_help(char *args);

static int cmd_si(char *args) {
	int n;

	if (args && sscanf(args,"%d",&n)==1)
		cpu_exec(n);
	else
		cpu_exec(1);

  return 0;
}

static int cmd_info(char *args) {
	if (!args) {
		printf("Unknown command\n");
		return 0;
	}

	char subcmd;

	if (sscanf(args, "%c", &subcmd)==1) {
		switch (subcmd) {
			case 'r':
				isa_reg_display();
				break;

			case 'w':
				info_watchpoints();
				break;

			default:	
				printf("Unknown command\n");
				break;
		}
	}
	return 0;
}

static int cmd_x(char *args) {
	int n, i;
	vaddr_t addr;
	if (args && (sscanf(args, "%d 0x%x", &n, &addr))) {
		printf("n=%d, addr=0x%x\n", n, addr);
	}

	for(i=0; i<n; i++){
		printf("@0x%x: 0x%x\n",addr  ,vaddr_read(addr, 4));
		addr += 4;
	}
	return 0;
}

static int cmd_p(char *args) {
	if (!args) {
		printf("No expression provided\n");
		return 0;
	}

	init_regex();
	bool success;
	uint32_t result;
	result = expr(args,&success);
	if(success){
		printf("ans: %u (0x%x)\n", result, result);
	}else{
		printf("Invalid expression\n");
	}

	return 0;
}

static int cmd_w(char *args) {
	create_watchpoint(args);
	return 0;
}

static int cmd_d(char *args) {
	int id;
	if(sscanf(args, "%d", &id)==1){
		delete_watchpoint(id);
	}
	return 0;
}



static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute a step of instruction for <N> times", cmd_si},
	{ "info", "Print registers' status with [r], watchpoints' info with [w]", cmd_info },
	{ "x", "Print memory content at [EXPR] for [N]*4 bytes (x [N] [EXPR])", cmd_x},
	{ "p", "Evaluate expression [EXPR]", cmd_p},
	{ "w", "Create watchpoint for expression [EXPR]", cmd_w},
	{ "d", "Delete watchpoint [N]", cmd_d}

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
