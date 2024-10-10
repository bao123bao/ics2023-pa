#include <common.h>
#include "syscall.h"

#define CONFIG_STRACE 1

int sys_yield() {
	yield();
	return 0;
}

int sys_exit(int status) {
	halt(status);
	return status;
}

void do_syscall(Context *c) {
  int ret_val = 0;
//	printf("STRACE=%d\n",CONFIG_STRACE);
	uintptr_t a[4];
  a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  switch (a[0]) {
		
		case SYS_yield:
			ret_val = sys_yield();
#ifdef CONFIG_STRACE
			printf("syscall: yield (args: %d, %d, %d, ret_val=%d)\n", 
				a[1], a[2], a[3], ret_val);
#endif
			break;

		case SYS_exit:
			ret_val = sys_exit(a[1]);
#ifdef CONFIG_STRACE
			printf("syscall: exit (args: %d, %d, %d, ret_val=%d)\n", 
				a[1], a[2], a[3], ret_val);
#endif
			break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
	// set a0 to return value
	c->GPRx = ret_val;
}
