#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#define BUF_SIZE 5000

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void num2str(char *buf, int *len, int num, bool is_hex) {
	int digit;
	int cnt = 0, p = 0;
	
	// to decimal num string
	if(!is_hex){

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
		t = buf[p]; buf[p] = buf[q]; buf[q] = t;
		p++; q--;
	}
	*len = cnt;
	
	} else { // to heximal num stirng
		
	uint32_t u_num = (uint32_t) num;
	
	buf[0] = '0'; buf[1] = 'x';
	cnt += 2; p = 2;

	while(u_num >= 16) {
		digit = u_num % 16;
		if(digit<=9) buf[cnt++] = '0' + digit;
		else         buf[cnt++] = 'a' + (digit - 10);
		u_num /= 16;
	}
	if(u_num <= 9) buf[cnt++] = '0' + u_num;
	else           buf[cnt++] = 'a' + (u_num - 10);
	buf[cnt] = '\0';
	//printf("initial str: %s\n",buf);

	int q = cnt-1;
	char t;

	// swap the digits to true order
	while(p<q){
		t = buf[p]; buf[p] = buf[q]; buf[q] = t; 
		p++; q--;
	}
	*len = cnt;
	
	}
	//printf("final str: %s\n", buf);
}


int printf(const char *fmt, ...) {
  va_list args;
	va_start(args, fmt);
	
	char buf[BUF_SIZE];
	int cnt = vsprintf(buf, fmt, args);
	int i;
	for(i=0; i<cnt; i++){
		putch(buf[i]);
	}
	va_end(args);
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
					num2str(p, &numlen, num, false);
					p += numlen;
					cnt += numlen;
					break;
				case 'c':
					char ch = (char) va_arg(ap, int);
					*p = ch;
					p++;
					cnt++;
					break;
				case 'p':
					uint32_t u_num = va_arg(ap, uint32_t);
					int u_numlen;
					num2str(p, &u_numlen, u_num, true);
					p += u_numlen;
					cnt += u_numlen;
					break;
				default:
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
}  


int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}


int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
