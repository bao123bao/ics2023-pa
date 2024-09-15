#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define RING_BUF_SIZE 10 
#define BUF_INST_SIZE 100

typedef struct {
	char * queue[RING_BUF_SIZE];
	int front;
	int rear;
	int size;
} ringbuf;

void init_ringbuf(ringbuf *pbuf);

void update_ringbuf(ringbuf *pbuf, char *info);

void enqueue_ringbuf(ringbuf *pbuf, char *info); 

void override_ringbuf(ringbuf *pbuf, char *info); 

void print_ringbuf(ringbuf *pbuf);
