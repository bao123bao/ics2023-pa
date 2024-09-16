#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <assert.h>
#include <common.h>

typedef struct {
	Elf32_Addr sym_addr;
	char sym_name[50];
} func_sym;

extern func_sym funcs[100];
extern int func_sym_len;

void read_func_symbols(char *filename, func_sym *funcs, int *len);

void print_func_syms(func_sym *funcs, int len);

int check_func_sym(word_t addr, func_sym *funcs, int len);
