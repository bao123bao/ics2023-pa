#include <common.h>
#include <fs.h>
#include <proc.h>
#include "syscall.h"

#define CONFIG_STRACE 1


int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

void naive_uload(PCB *pcb, const char *filename);

struct timeval {
	uint32_t tv_sec;
	uint32_t tv_usec;
};

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
//	printf("in sys_gettimeofday\n");
	if(!tv)
		return -1;

	AM_TIMER_UPTIME_T uptime = io_read(AM_TIMER_UPTIME);
	uint64_t us = uptime.us;
	tv->tv_sec  = us / 1000000;
	tv->tv_usec = us % 1000000;
//	printf("uptime.us=%d, tv_sec=%d, tv_usec=%d\n", 
//		(int)us, (int)tv->tv_sec, (int)tv->tv_usec);
	return 0;
}

int sys_yield() {
	yield();
	return 0;
}


int sys_write(int fd, const void *buf, size_t count) {
	return fs_write(fd, buf, count);
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

int sys_lseek(int fd, size_t offset, int whence) {
	return fs_lseek(fd, offset, whence);
}

int sys_close(int fd) {
	return fs_close(fd);
}

int sys_execve(const char *pathname, char *const argv[], char *constenvp[]) {
	naive_uload(NULL, pathname);
	return 0;
}

int sys_exit(int status) {
	//halt(status);
	sys_execve("/bin/menu", NULL, NULL);
	return status;
}

void do_syscall(Context *c) {
  int ret_val = 0;
	
	uintptr_t a[4];
  a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  switch (a[0]) {

		case SYS_execve:
#ifdef CONFIG_STRACE
			printf("syscall: execve (args: %s, NULL, NULL)\n", a[1]);
#endif
			ret_val = sys_execve((const char *)a[1], NULL, NULL);
			break;

		case SYS_gettimeofday:
	//		printf("switch to SYS_gettime\n");
			ret_val = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]);
#ifdef CONFIG_STRACE
			printf("syscall: gettimeofday (args: %p, %p, ret_val=%d)\n", a[1], a[2], ret_val);
#endif
			break;

		case SYS_brk:
			ret_val = sys_brk((void *)a[1]);
#ifdef CONFIG_STRACE
			printf("syscall: brk (args: %p, ret_val=%d)\n", a[1], ret_val);
#endif
			break;

		case SYS_yield:
#ifdef CONFIG_STRACE
			printf("syscall: yield (args: none)\n");
#endif
			ret_val = sys_yield();
			break;

		case SYS_exit:
#ifdef CONFIG_STRACE
			printf("syscall: exit (args: %d)\n", a[1]);
#endif
			ret_val = sys_exit(a[1]);
			break;
		
		case SYS_open:
			ret_val = sys_open((const char *)a[1], a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: open (args: %p %d, %d), ret_val=%d<%s>,)\n", 
				a[1], a[2], a[3], ret_val, file_table[ret_val].name);
#endif
			break;

		case SYS_read:
			ret_val = sys_read(a[1], (void *)a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: read (args: %d<%s>, %p, %d, ret_val=%d)\n", 
				a[1], file_table[a[1]].name, a[2], a[3], ret_val);
#endif
			break;

		case SYS_write:
			ret_val = sys_write(a[1], (void *)a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: write (args: %d<%s>, %p, %d, ret_val=%d)\n", 
				a[1], file_table[a[1]].name, a[2], a[3], ret_val);
#endif
			break;
		
		case SYS_lseek:
			ret_val = sys_lseek(a[1], a[2], a[3]);
#ifdef CONFIG_STRACE
			printf("syscall: lseek (args: %d<%s>, %d, %d, ret_val=%d)\n", 
				a[1], file_table[a[1]].name, a[2], a[3], ret_val);
#endif
			break;

		case SYS_close:
			ret_val = sys_close(a[1]);
#ifdef CONFIG_STRACE
			printf("syscall: close (args: %d<%s>, ret_val=%d)\n", 
				a[1], file_table[a[1]].name, ret_val);
#endif
			break;
			
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
	// set a0 to return value
	c->GPRx = ret_val;
}
