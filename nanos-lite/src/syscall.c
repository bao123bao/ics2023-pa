#include <common.h>
#include "syscall.h"

void sys_yield() {
	yield();
}

void sys_exit(int status) {
	halt(status);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  switch (a[0]) {
		case SYS_yield:
			printf("do sys_yield syscall\n");
			sys_yield();
			break;
		case SYS_exit:
			printf("do sys_exit syscall\n");
			sys_exit(a[1]);
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
