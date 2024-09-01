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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
//#include "../../../include/isa.h"
#include <regex.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "stack.h"

#define MAX_TOKENS_ARR_LEN 30 
#define MAX_TOKEN_LEN 50


bool debug_flag = true;
//bool stdin_flag = false;
int correct_cnt = 0;


enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ, TK_NUMBER, TK_EMPTY, TK_NSIGN, TK_DEREF, TK_AND, TK_HEX, TK_REG

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
	{"-", '-'},           // minus
	{"\\*", '*'},         // multiply
	{"/", '/'},           // divide
  {"==", TK_EQ},        // equal
	{"\\(", '('},         // left bracket
	{"\\)", ')'},         // right bracket
	{"[0-9]+", TK_NUMBER},// decimal number
	{"!=", TK_NEQ},       // not equal !=
	{"&&", TK_AND},       // logical and
	{"0x[0-9]*", TK_HEX}, // hex number 0x123123
	{"\\$[0-9a-z]*", TK_REG} // register names $reg
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[MAX_TOKEN_LEN];
} Token;

static Token tokens[MAX_TOKENS_ARR_LEN] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

	for (i=0; i<MAX_TOKENS_ARR_LEN; i++) {
		tokens[i].type = TK_EMPTY;
	}

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        /* Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start); */

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
						// printf("spaces read, pass\n");
						break;

					case TK_NUMBER:
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						tokens[nr_token].str[substr_len] = '\0';
						tokens[nr_token].type = TK_NUMBER;
						// printf("tokens[%d] type=number: %s\n", nr_token, tokens[nr_token].str);
						nr_token++;
						break;

					default:
						tokens[nr_token].type = *substr_start;
						// printf("tokens[%d].type='%c'\n", nr_token, tokens[nr_token].type);
						nr_token++;
						break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


// return: true for valid parens

bool check_valid_paren(int p, int q){
	int acc=0, type;
	if(p==q && tokens[p].type==TK_NUMBER)
		return true;
	while (p<=q) {
		type = tokens[p].type;
		switch(type){
			case '(':
				acc++;
				break;
			case ')':
				acc--;
				break;
			default:
				break;
		}
		p++;
	}
	if(acc==0)
		return true;
	else
		return false;

}


int check_parentheses(int p, int q) {
	if(debug_flag){
		printf("check_paren: p=%d, q=%d\n",p,q);
	}
	
	if ( !(tokens[p].type=='(' && tokens[q].type==')') )
		return 0;
	if (q-p==1 || q-p==2)
		return 1;
	
	// already checked initial p and q is '(' and ')'
	int acc=1, type;
	p++;
	
	while (p<q) {
		type = tokens[p].type;
		switch(type){
			case '(':
				acc++;
				break;
			case ')':
				acc--;
				if(acc==0)
					return 0;
				break;
			default:
				break;
		}
		p++;
	}
	return 1;
	
}


int op_position(int p, int q) {

	if(debug_flag)
		printf("op_position(): p=%d, q=%d\n",p,q);
	int cur_op_pos = -1;
	int cur_op_type = -1;
	int paren_flag = -1;
	int cur_type;
  //	while (tokens[p].type==TK_NUMBER)

	while (p <= q) {
		if(debug_flag)
			printf("in op_pos while loop: p=%d, q=%d\n", p,q);

		cur_type = tokens[p].type;

		if(debug_flag && cur_op_type>0 && cur_type<256)
			printf("p=%d, cur_type=%c, cur_op='%c'@%d\n", p, cur_type,cur_op_type, cur_op_pos );

		switch (cur_type) {
			// current ptr's type

			case '(':
				// enter paren area
				paren_flag++;
				break;

			case ')':
				// exit paren area
				paren_flag--;
				break;

			case '+':
			case '-':
				// currently meet op +-
				switch (cur_op_type) {
					// found op type

					case -1:
					case '+':
					case '-':
					case '*':
					case '/':
						// no previous op or previously +-
						if (paren_flag<0) {
							// when not in paren area
							cur_op_pos = p;
							cur_op_type = cur_type;
						}
						break;
					default:
						break;
				}
				break;

			case '*':
			case '/':
				// currently meet op 
				switch (cur_op_type) {
					case -1:
					case '*':
					case '/':
						if (paren_flag<0) {
							cur_op_pos = p;
							cur_op_type = cur_type;
						}
						break;

					case '+':
					case '-':
						if (paren_flag<0) {
							//cur_op_pos = p;
							//cur_op_type = cur_type;
						}
						break;
					default:
						break;
				}
				break;

		}
		p++;
	}	
	return cur_op_pos;
}


bool error_flag = false;

int eval(int p, int q) {
	if (debug_flag) {
		printf("eval: p=%d, q=%d\n", p, q);
	}
	if (p > q) {
		// bad expr
		if (debug_flag)
			printf("bad expr\n");
		assert(0);
		return -1;

	}else if(p == q) {
		// single number
		return atoi(tokens[p].str);

	}else if(check_parentheses(p, q) == 1) {
		// outer parentheses
		if (debug_flag){
			printf("outer paren (p=%d, q=%d)\n",p,q);
		}
		return eval(p + 1, q - 1);
	}else if(tokens[p].type == TK_NSIGN){
		return eval(p+1, q);
	}else{
		int op_pos = op_position(p, q);
		if(debug_flag)
			printf("eval: po_pos=%d\n", op_pos);
		int val1 = eval(p, op_pos - 1);
		int val2 = eval(op_pos + 1, q);
		if (val1==INT_MIN || val2==INT_MIN)
			return INT_MIN;
		switch (tokens[op_pos].type) {
			case ('+'): return val1 + val2;
			case ('-'): return val1 - val2;
			case ('*'): return val1 * val2;
			case ('/'): {
				if (val2==0) {
					error_flag = true;
					return INT_MIN;	
				}
				return val1 / val2;
			}
			default: assert(0); return -1;
		}
	}
}

// return a^n given n>=0
int power(int a, int n) {
	if (n==0) 
		return 1;
	int i;
	int result = a;
	for (i=1; i<n ;i++) {
		result *= a;
	}
	return result;
}

void print_tokens(int len) {
	int i, type;
	for (i=0; i<MAX_TOKENS_ARR_LEN; i++) {
		type = tokens[i].type;
		switch (type) {
			case TK_NUMBER:
				printf("#%d=NUM(%s) ", i, tokens[i].str);
				break;
			case TK_EMPTY:
				//printf("#%d=EMPTY ", i);
				break;
			case TK_NSIGN:
				printf("#%d=NSIGN ", i);
				break;
			default:
				printf("#%d=%c ", i, type);
		}	
	}
	putchar('\n');
}


word_t expr(char *e, bool *success) {
	
	/*FILE *fp;
	char *line = NULL;
	size_t slen = 0;
	ssize_t read;

	fp = fopen("/home/bao/ics-2023/nemu/src/monitor/sdb/output", "r");
	if (!fp) {
		printf("No file found\n");
		return 0;
	}
	int cnt = 0;
	int ans;
	char expr[65536];
	while ((read = getline(&line, &slen, fp)) != -1) {
		sscanf(line, "%d %[^\t\n]", &ans, expr);


		if(debug_flag) {
			//printf("raw expr: %s\n", expr);
			printf("a: %d\nexpr: %s\n", ans, expr);
		}*/

		if (!make_token(e)) {
    	*success = false;
    	return 0;
 		}

		
	int i;
	int len = 0;
	int type;

	// calculate tokens length
	for (i=0; i<MAX_TOKENS_ARR_LEN; i++)
		if (tokens[i].type != TK_EMPTY){
			len++;
		}


	// check paren
	if (!check_valid_paren(0, len-1)) {
		if (debug_flag)
			printf("invalid parentheses\n");
		*success = false;
		return 0;
	}
	
	/*
	if (debug_flag) {
		print_tokens(len);
	}
	*/

	// extract dereference from multiply
	for (i=0; i<MAX_TOKENS_ARR_LEN; i++) {
		if (tokens[i].type=='*') {
			if (i==0){
				tokens[i].type = TK_DEREF;
			}else{
				switch(tokens[i-1].type){
					case '+':
					case '-':
					case '*':
					case '/':
					case '(':
					case TK_EQ:
					case TK_NEQ:
						tokens[i].type = TK_DEREF;
						break;
					default:
						break;
				}
			}
		} 
	}

	if (debug_flag) {
		print_tokens(len);
	}	
	
	return 0;

	// extract negative sign from minus
	bool next_nsign = true;
	int nsign_times = 0;
	int num;
	for (i=0; i<MAX_TOKENS_ARR_LEN; i++) {
		type = tokens[i].type;
		switch (type) {
			case TK_NUMBER:
				// next is op or ')', not negative sign
				next_nsign = false;
				num = atoi(tokens[i].str); // original unsigned number
				num = num * power(-1 , nsign_times); // signed number
				sprintf(tokens[i].str, "%d", num); // cast back to string
				nsign_times = 0;
				break;

			case '(':
				// next '-' is negative sign
				next_nsign = true;
				break;

			case ')':
				// next '-' is op minus
				next_nsign = false;
				break;

			case '+':
			case '*':
			case '/':
				// these signs are op
				next_nsign = true;
				break;

			case '-':
				if (!next_nsign) {
					// this '-' is not negative sign
					// remain original
					next_nsign = true;
				}else{
					// previous is op or '('
					// this '-' is negative sign
					tokens[i].type = TK_NSIGN;
					nsign_times++;
					next_nsign = true;
				}
				break;
			default: ;
		}
	}


	
	// print tokens after dealing with minus
	
	if (debug_flag) {
		print_tokens(len);
	}	

	int op_pos = op_position(0, len-1);
	if (debug_flag) {
		printf("main operator is %c at [%d]\n", tokens[op_pos].type ,op_pos);
	}
	
	int result = eval(0, len-1);
	if (result==INT_MIN && error_flag) {
		printf("error: divided by 0\n");
		return 0;
	}
	printf("result=%d\n",result);
	*success = true;

	//cnt++;
	/*
	if (result == ans) {
		correct_cnt++;
	}else{
		printf("error@#%d, expr: %s\n", cnt, expr);
	}
	//printf("cnt:%d, %d\n", cnt, result==ans);

	}

	printf("Accuracy: %d/%d is correct\n", correct_cnt, cnt);
	free(line);
	*/
	return 0;
	
}


