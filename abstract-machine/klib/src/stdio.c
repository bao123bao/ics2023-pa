#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

void int2str(char *buf, int *len, int num) {
	int digit;
	int cnt = 0, p = 0;

	if(num<0){
		buf[p] = '-';
		p++;
		cnt++;
	}

	while(num > 10) {
		digit = num % 10;
		buf[cnt++] = '0' + digit;
		num /= 10;
	}
	buf[cnt++] = '0' + num;
	buf[cnt] = '\0';
	//printf("initial str: %s\n",buf);

	int q = cnt-1;
	char t;

	while(p<q){
		t = buf[p];
		buf[p] = buf[q];
		buf[q] = t;
		p++;
		q--;
		cnt++;
	}
	*len = cnt;
	//printf("final str: %s\n", buf);
}



int sprintf(char *out, const char *fmt, ...) {
  va_list args;
	va_start(args, fmt);

	char fmt_type;
	char c;
	char *p = out;
	
	int i=0;
	int fmt_cnt = 0;
	int len = strlen(fmt);
	
	while(i<len){
		c = fmt[i];
		if(c=='%' && i<len-1){
			fmt_type = fmt[i+1];

			switch (fmt_type) {
				case 's':
					char *sp = va_arg(args, char*);
					strcpy(p, sp);
					p += strlen(sp);
					fmt_cnt++;
					break;
				case 'd':
					int num = va_arg(args, int);
					int numlen;
					int2str(p, &numlen, num);
					p += numlen;
					fmt_cnt++;
					break;
				default:
					assert(0);
			}

			i += 2;

		}else{
			*p = c;
			p++;
			i++;
		}
	}
	va_end(args);
	*p = '\0';
	return fmt_cnt;
}


int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}


int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
