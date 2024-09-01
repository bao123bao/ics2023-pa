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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#define MAX_EXPR_LEN 65536
#define MAX_CODE_LEN (MAX_EXPR_LEN + 128)

// this should be enough
// buf stores expression
static char buf[MAX_EXPR_LEN + 1] = {};
static char* pbuf = buf;
static bool overflow = false;
// code_but stros expr + main_function
static char code_buf[MAX_CODE_LEN] = {}; // a little larger than `buf`
static const bool debug_flag = false;

// main function
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  int result = %s; "
"  printf(\"%%d\", result); "
"  return 0; "
"}";


// return rand number [0, n-1]
int choose(int n) {
	return rand() % n;
}

static void print_debug(char *s) {
	if (debug_flag)
		printf("%s made (dif=%ld, of=%d): %s\n",s, pbuf-buf, overflow, buf);
}


static void gen_space() {
	if (pbuf-buf >= 0.7*MAX_EXPR_LEN) {
		overflow = true;
		return;
	}
	if (!choose(5) && !overflow) {
		*pbuf = ' ';
		*(pbuf+1) = '\0';
		pbuf++;
		print_debug("space");
	}
}

static void gen_num() {
	if (pbuf-buf >= 0.7*MAX_EXPR_LEN) {
		overflow = true;
		return;
	}
	int num = choose(2000)-500;
	int len = sprintf(pbuf, "%d", num);
	pbuf += len;
	gen_space();
	print_debug("num");
}

static void gen(char c) {
	if (pbuf-buf >= 0.7*MAX_EXPR_LEN) {
		overflow = true;
		return;
	}
	*pbuf = c;
	*(pbuf+1) = '\0';
	pbuf++;
	gen_space();
	print_debug("char");
}

static char ops[] = {'+', '-', '*', '/'};

static void gen_rand_op() {
	if (pbuf-buf >= 0.7*MAX_EXPR_LEN ) {
		overflow = true;
		return;
	}
	*pbuf = ops[choose(4)];
	*(pbuf+1) = '\0';
	pbuf++;
	gen_space();
	print_debug("op");
}

static void gen_rand_expr() {
	if (pbuf-buf >= 0.7*MAX_EXPR_LEN) {
		overflow = true;
		return;
	}
	switch (choose(3)) {
		case 0: 
			gen_num(); 
			break;
		case 1: 
			gen('('); 
			gen_rand_expr(); 
			gen(')'); 
			break;
		default: 
			gen_rand_expr();
			gen_rand_op();
			gen_rand_expr();
			break;
	}
}


int main(int argc, char *argv[]) {
 	int seed = time(0); 
	srand(seed);	

  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
	pbuf = buf;
  for (i = 0; i < loop; i ++) {
    
		overflow = false;
		pbuf = buf;
		*pbuf = '\0';
		gen_rand_expr();
		
		while ( (overflow || strlen(buf)<10) ) {
			if(debug_flag)	
				printf("overflow=%d, len=%ld\n", overflow, strlen(buf));
			
			overflow = false;
			pbuf = buf;
			*pbuf = '\0';
			gen_rand_expr();
		}

		// printf("#1: %s\n",buf);
	
    sprintf(code_buf, code_format, buf);
		
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -s -Werror -o /tmp/.expr");
		// printf("ret val of sys(gcc): %d\n", ret);
    if (ret != 0) {
			if(debug_flag)
				printf("gcc error happened! Abort!\n");
			continue; // error happens
		}

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL); // shoud have opened the file

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

		if(debug_flag)
	    printf("%d/%d: %d %s\n", i+1, loop, result, buf);
		else
	    printf("%d %s\n", result, buf);

  }

  return 0;
}
