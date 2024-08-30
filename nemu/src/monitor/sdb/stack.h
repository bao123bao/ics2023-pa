typedef struct {
	const int size;
	char *item;
	int top;
} Stack;

bool init_stack(Stack **ppStack, int size);
bool stack_is_empty(Stack *pStack);
bool stack_is_full(Stack *pStack);
bool stack_push(Stack *pStack, char item);

