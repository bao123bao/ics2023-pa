#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"


bool init_stack(Stack **ppStack, int size) {
	if (size <= 0) {
		printf("invalid stack size\n");
		return false;
	}
	*ppStack = (Stack *) malloc(sizeof(Stack));
	(*pStack)->size = size;
	(*pStack)->item = (char *) malloc(size * sizeof(char));
	(*pStack)->top = 0;
	return true;
}

bool stack_is_empty(Stack *pStack) {
	return (pStack->top == 0);
}

bool stack_is_full(Stack *pStack) {
	return (pStack->top == (pStack->size - 1));
}

bool stack_push(Stack *pStack, char item) {
	
}

