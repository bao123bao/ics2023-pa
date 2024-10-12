#include <common.h>
#include "syscall.h"

#define CONFIG_STRACE 1


int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);




int sys_yield() {
	yield();
	return 0;
}

int sys_exit(int status) {
	halt(status);
	return status;
}

int sys_write(int fd, const void *buf, size_t count) {
	
	return fs_write(fd, buf, count);
/*
	if(fd==1 || fd==2){
		int i;
		char *chars = (char *)buf;
		int cnt = 0;
		for(i=0; i<count; i++){
			putch(chars[i]);
			cnt++;
		}
		return cnt;
	}
	return -1;
	*/
}

extern char _end;

int sys_brk(void *addr){
	return 0;
}

int sys_open(const char *pathname, int flags, int mode) {
	return fs_open(pathname, flags, mode);
}

int sys_read(int fd, void *buf, size_t count) {
	return fs_read(fd, buf, count);
}
/*
int sys_write(int fd, const void *buf, size_t count) {
	return fs_write(fd, buf, count);
}
*/
int sys_lseek(int fd, size_t offset, int whence) {
	return fs_lseek(fd, offset, whence);
}

int sys_close(int fd) {
	return fs_close(fd);
}

void do_syscall(Context *c) {
  int ret_val = 0;
	
	uintptr_t a[4];
  a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  switch (a[0]) {
		case SYS_brk:
			ret_val = sys_brk((void *)a[1]);
			//char *s = 
#ifdef CONFIG_STRACE
			printf("syscall: brk (args: %d, ret_val=%d)\n", a[1], ret_val);
#endif
			break;

		case SYS_yield:
			ret_val = sys_yield();
#ifdef CONFIG_STRACE
			printf("syscall: yield (args: none, ret_val=%d)\n",  ret_val);
#endif
			break;

		case SYS_exit:
			ret_val = sys_exit(a[1]);
#ifdef CONFIG_STRACE
			printf("syscall: exit (args: %d, ret_val=%d)\n", a[1], ret_val);
#endif
			break;
		/*
		case SYS_write:
			ret_val = sys_write(a[1], (void *)a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: write (args: %d, %d, %d, ret_val=%d)\n", 
				a[1], a[2], a[3], ret_val);
#endif
			break;
		*/
		case SYS_open:
			ret_val = sys_open((const char *)a[1], a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: open (args: %d, %d, %d, ret_val=%d)\n", 
				a[1], a[2], a[3], ret_val);
#endif
			break;

		case SYS_read:
			ret_val = sys_read(a[1], (void *)a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: read (args: %d, %d, %d, ret_val=%d)\n", 
				a[1], a[2], a[3], ret_val);
#endif
			break;

		case SYS_write:
			ret_val = sys_write(a[1], (void *)a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: write (args: %d, %d, %d, ret_val=%d)\n", 
				a[1], a[2], a[3], ret_val);
#endif
			break;
		
		case SYS_lseek:
			ret_val = sys_lseek(a[1], a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: lseek (args: %d, %d, %d, ret_val=%d)\n", 
				a[1], a[2], a[3], ret_val);
#endif
			break;

		case SYS_close:
			ret_val = sys_close(a[1]);
#ifdef CONFIG_STRACE
			printf("syscall: lseek (args: %d, ret_val=%d)\n", a[1], ret_val);
#endif
			break;
			
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
	// set a0 to return value
	c->GPRx = ret_val;
//	printf("ret_val=%d\n");
}
