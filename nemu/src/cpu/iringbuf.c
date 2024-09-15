#include "iringbuf.h"

void init_ringbuf(ringbuf *pbuf) {
	pbuf->front = 0;
	pbuf->rear = 0;
	pbuf->size = 0;

	int i;
	for(i=0; i<RING_BUF_SIZE; i++) {
		pbuf->queue[i] = (char *)malloc(BUF_INST_SIZE * sizeof(char));
	}
}

void update_ringbuf(ringbuf *pbuf, char *info){
	if(pbuf->size < RING_BUF_SIZE){
		// queue is not full
		//printf("enqueue\n");
		enqueue_ringbuf(pbuf, info);
	}else{
		// queue is full, override earlist info
		//printf("override\n");
		override_ringbuf(pbuf, info);
	}
}

void enqueue_ringbuf(ringbuf *pbuf, char *info) {
	strcpy(pbuf->queue[pbuf->rear], info);
	pbuf->rear = (pbuf->rear + 1) % RING_BUF_SIZE;
	pbuf->size++;
}

void override_ringbuf(ringbuf *pbuf, char *info) {
	strcpy(pbuf->queue[pbuf->front], info);
	pbuf->rear = (pbuf->rear + 1) % RING_BUF_SIZE;
	pbuf->front = (pbuf->front + 1) % RING_BUF_SIZE;
}

void print_ringbuf(ringbuf *pbuf){
	printf("\niringbuf asm log:\n");
	if(pbuf->size==0)
		return;
	int i = pbuf->front;
	do{
		printf("%s\n", pbuf->queue[i]);
		i = (i+1) % RING_BUF_SIZE;
	}while(i != pbuf->rear);

	putchar('\n');
}
