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
#include "stack.h"

#define MAX_TOKENS_ARR_LEN 50 
#define MAX_TOKEN_LEN 32

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUMBER, TK_EMPTY

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
	{"[0-9]+", TK_NUMBER} // decimal number
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


// return: 1 for outer paren, 0 for valid paren, -1 for bad expr
int check_parentheses(int p, int q) {
	Stack *pStack;
	char type;

	// whether the string like: "( ... )"
	bool closed_paren = (tokens[p].type=='(' && tokens[q].type==')');

	init_stack(&pStack, MAX_TOKENS_ARR_LEN);

	while (p <= q) {
		if (tokens[p].type == '(') {
			stack_push(pStack, '('); 
		}else if (tokens[p].type == ')') {
			if (!stack_pop(pStack, &type))
				return -1;
			if (type != '(') {
				assert(0);
				return -1;
			}
		}
		p++;
	}

	if (stack_is_empty(pStack) && closed_paren)
		return 1;
	else if (stack_is_empty(pStack))
		return 0;
	else 
		return -1;
}


int op_position(int p, int q) {
	int cur_op_pos = -1;
	int cur_op_type = -1;
	bool paren_flag = false;
	int cur_type;
  //	while (tokens[p].type==TK_NUMBER)

	while (p <= q) {
		cur_type = tokens[p].type;
		switch (cur_type) {

			case '(':
				// enter paren area
				paren_flag = true;
				break;

			case ')':
				// exit paren area
				paren_flag = false;
				break;

			case '+':
			case '-':
				// currently meet op +-
				switch (cur_op_type) {
					case -1:
					case '+':
					case '-':
						// no previous op or previously +-
						if (!paren_flag) {
							// when not in paren area
							cur_op_pos = p;
							cur_op_type = cur_type;
						}
						break;
					case '*':
					case '/':
					default:
						break;
				}
				break;

			case '*':
			case '/':
				// currently meet op */
				switch (cur_op_type) {
					case -1:
					case '*':
					case '/':
						if (!paren_flag) {
							cur_op_pos = p;
							cur_op_type = cur_type;
						}
						break;
					case '+':
					case '-':
					default:
						break;
				}
				break;

		}
		p++;
	}	
	return cur_op_pos;
}


int eval(int p, int q) {
	if (p > q) {
		// bad expr
		printf("bad expr\n");
		assert(0);
		return -1;

	}else if(p == q) {
		// single number
		return atoi(tokens[p].str);
	}else if(check_parentheses(p, q) == 1) {
		// outer parentheses
		return eval(p + 1, q - 1);
	}else if(check_parentheses(p, q) == -1) {
		// bad expr
		printf("invalid parentheses\n");
		assert(0);
		return -1;
	}else{
		int op_pos = op_position(p, q);
		int val1 = eval(p, op_pos - 1);
		int val2 = eval(op_pos + 1, q);
		switch (tokens[op_pos].type) {
			case ('+'): return val1 + val2;
			case ('-'): return val1 - val2;
			case ('*'): return val1 * val2;
			case ('/'): {
				if (val2==0) {
					
				}
				return val1 / val2;
			}
			default: assert(0); return -1;
		}
	}
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
	
  /* TODO: Insert codes to evaluate the expression. */
	int i;
	int len = 0;
	int type;
	// print tokens
	for (i=0; i<MAX_TOKENS_ARR_LEN; i++) {
		type = tokens[i].type;
		if (type != TK_EMPTY) {
			len++;
			switch (type) {
				case TK_NUMBER:
					printf("tk=NUM(%s) ", tokens[i].str);
					break;
				case TK_EMPTY:
					printf("tk=EMPTY ");
					break;
				default:
					printf("tk=%c ", type);
			}
		}
	}
	printf("\ntokens len=%d\n", len);
	
	// check paren
	int paren_flag = check_parentheses(0, len-1); 
	if (paren_flag == -1) {
		printf("invalid parentheses (flag=%d)\n", paren_flag);
		*success = false;
		return 0;
	}

	int op_pos = op_position(0, len-1);
	printf("main operator is %c at [%d]\n", tokens[op_pos].type ,op_pos);
	
	int result = eval(0, len-1);
	printf("result is %d\n", result);

  return 0;
}


