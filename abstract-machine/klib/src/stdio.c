#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <limits.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void int2str(char *buf, int *len, int num) {
	int digit;
	int cnt = 0, p = 0;
	
	if(num == INT_MIN){
		char *temp = "-2147483648";
		int i;
		for(i=0; i<11; i++){
			buf[i] = temp[i];
		}
		buf[i] = '\0';
		*len = 11;
		return;
	}


	if(num<0){
		num = -num;
		buf[p] = '-';
		p++;
		cnt++;
	}

	while(num >= 10) {
		digit = num % 10;
		buf[cnt++] = '0' + digit;
		num /= 10;
	}
	buf[cnt++] = '0' + num;
	buf[cnt] = '\0';
	//printf("initial str: %s\n",buf);

	int q = cnt-1;
	char t;

	// swap the digits to true order
	while(p<q){
		t = buf[p];
		buf[p] = buf[q];
		buf[q] = t;
		p++;
		q--;
	}
	*len = cnt;
	//printf("final str: %s\n", buf);
}


int printf(const char *fmt, ...) {
  va_list args;
	va_start(args, fmt);
	
	char buf[1000];
	int cnt = vsprintf(buf, fmt, args);
	int i;
	for(i=0; i<cnt; i++){
		putch(buf[i]);
	}
	
	return cnt;
}

int vsprintf(char *out, const char *fmt, va_list ap) {

	char fmt_type;
	char c;
	char *p = out;
	
	int i=0;
	int cnt = 0;
	int len = strlen(fmt);
	
	while(i<len){
		c = fmt[i];
		if(c=='%' && i<len-1){
			fmt_type = fmt[i+1];

			switch (fmt_type) {
				case 's':
					char *sp = va_arg(ap, char*);
					strcpy(p, sp);
					int slen = strlen(sp);
					p += slen;
					cnt += slen;
					break;
				case 'd':
					int num = va_arg(ap, int);
					int numlen;
					int2str(p, &numlen, num);
					p += numlen;
					cnt += numlen;
					break;
				case 'c':
					char ch = (char) va_arg(ap, int);
					*p = ch;
					p++;
					cnt++;
					break;
				default:
					putch(c);
					putch(fmt_type);
					assert(0);
			}
			i += 2;
		}else{
			*p = c;
			p++;
			i++;
			cnt++;
		}
	}
	*p = '\0';
	return cnt;
}



int sprintf(char *out, const char *fmt, ...) {
 	va_list args;
	va_start(args, fmt);
	
	int cnt = vsprintf(out, fmt, args);
	va_end(args);
	
  return cnt;
  
  
  /*
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
	*/
}


int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}


int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
