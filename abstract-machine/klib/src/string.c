#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
	while(*s != '\0') {
		len++;
		s++;
	}
	return len;
}

char *strcpy(char *dst, const char *src) {
	const char *p = src;
	char *q = dst;
	while(*p != '\0') {
		*q = *p;
		q++;
		p++;
	}
	*q = '\0';
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
	int cnt = 0;
	const char *p = src;
	char *q = dst;

	while(cnt<n || *p!='\0'){
		*q = *p;
		q++;
		p++;
		cnt++;
	}

	while(cnt<n){
		*q = '\0';
		cnt++;
	}

	return dst;
}

char *strcat(char *dst, const char *src) {
	strcpy(dst+strlen(dst), src);
	return dst;
}

int strcmp(const char *s1, const char *s2) {
  const char *p = s1;
	const char *q = s2;
	while(*p!='\0' && *q!='\0'){
		if(*p != *q){
			return (*p) - (*q);
		}
		p++;
		q++;
	}
	return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  const char *p = s1;
	const char *q = s2;
	int cnt = 0;

	while(cnt<n && *p!='\0' && *q!='\0'){
		if(*p != *q){
			return (*p) - (*q);
		}
		p++;
		q++;
		cnt++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
  unsigned char *p = (unsigned char *) s;
	unsigned char content = (unsigned char) c;
	int i;
	for(i=0; i<n; i++){
		p[i] = content;
	}
	return s;
}

void *memmove(void *dst, const void *src, size_t n) {
	char temp[n];
	memcpy(temp, src, n);
	memcpy(dst, temp, n);
	return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *q = (char *) out;
	const char *p = (char *) in;
	int i;
	for(i=0; i<n; i++){
		q[i] = p[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p = (unsigned char *)s1;
  const unsigned char *q = (unsigned char *)s2;
	int cnt = 0;
	while(cnt < n){
		if(*p != *q){
			return (int) ((*p) - (*q));
		}
		p++;
		q++;
		cnt++;
	}
	return 0;
}

#endif
