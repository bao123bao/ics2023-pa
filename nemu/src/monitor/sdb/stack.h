#include <stdbool.h>

typedef struct {
	int size;
	char *items;
	int top;
} Stack;

bool init_stack(Stack **ppStack, int size);
bool stack_is_empty(Stack *pStack);
bool stack_is_full(Stack *pStack);
bool stack_push(Stack *pStack, char item);
bool stack_pop(Stack *pStack, char *pItem);
void stack_print(Stack *pStack);

