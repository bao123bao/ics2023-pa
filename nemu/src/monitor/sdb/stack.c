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
	(*ppStack)->size = size;
	(*ppStack)->items = (char *) malloc(size * sizeof(char));
	(*ppStack)->top = -1;
	return true;
}

bool stack_is_empty(Stack *pStack) {
	return (pStack->top == -1);
}

bool stack_is_full(Stack *pStack) {
	return (pStack->top == (pStack->size - 1));
}

bool stack_push(Stack *pStack, char item) {
	if (stack_is_full(pStack)) {
		printf("stack is full\n");
		return false;
	}
	pStack->top++;
	pStack->items[pStack->top] = item;
	return true;
}

bool stack_pop(Stack *pStack, char *pItem) {
	if (stack_is_empty(pStack)) {
		printf("stack is empty\n");
		return false;
	}
	*pItem = pStack->items[pStack->top];
	pStack->top--;
	return true;
}

void stack_print(Stack *pStack) {
	if (!pStack) {
		printf("null stack ptr\n");
		return;
	}
	if (stack_is_empty(pStack)) {
		printf("empty stack\n");
		return;
	}
	int i;
	int size = pStack->size;
	printf("Stack size=%d\n", size);
	printf("Bottom: ");
	for (i=0; i<size; i++) {
		printf("%c ", pStack->items[i]);
	}
	printf("\n");
	if (stack_is_full(pStack))
		printf("stack is full\n");
	return;
}

