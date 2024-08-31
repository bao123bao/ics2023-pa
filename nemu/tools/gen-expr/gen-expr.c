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
static char buf[MAX_EXPR_LEN] = {};
static char* pbuf = buf;
static bool overflow = false;
// code_but stros expr + main_function
static char code_buf[MAX_CODE_LEN] = {}; // a little larger than `buf`

// main function
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


// return rand number [0, n-1]
int choose(int n) {
	return rand() % n;
}

static void gen_num() {
	if (pbuf-buf >= NAX_EXPR_LEN) {
		overflow = true;
		return;
	}
	int num = choose(100) + 1;
	int len = sprintf(pbuf, "%d", num);
	pbuf += len;
}

static void gen(char c) {
	if (pbuf-buf >= NAX_EXPR_LEN) {
		overflow = true;
		return;
	}
	*pbuf = c;
	*(pbuf+1) = '\0';
	pbuf++;
}

static char ops[] = {'+', '-', '*', '/'};

static void gen_rand_op() {
	if (pbuf-buf >= NAX_EXPR_LEN) {
		overflow = true;
		return;
	}
	*pbuf = ops[choose(4)];
	*(pbuf+1) = '\0';
	pbuf++;
}

static void gen_rand_expr() {
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
	*pbuf = '\0';
  srand(seed);
	overflow = false;
	gen_rand_expr();
	if (overflow) {
		printf("buf overflow, discard this expr\n");
		return 0;
	}
	printf("expr: %s\n", buf);
	/*
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
	*/
  return 0;
}
