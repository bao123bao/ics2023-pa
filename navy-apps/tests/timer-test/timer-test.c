#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

int main() {
  printf("Hello Timer!\n");
  
	struct timeval tv, last_tv;
	gettimeofday(&last_tv, NULL);
//	printf("start time: %d, %d\n", (int)last_tv.tv_sec, (int)last_tv.tv_usec);
	while (1) {
		gettimeofday(&tv, NULL);
	//	printf("tv-last = %d\n", tv.tv_usec - last_tv.tv_usec);
    if ( (tv.tv_sec-last_tv.tv_sec)*1000000 + (tv.tv_usec - last_tv.tv_usec) >= 500000) {
  	  printf("Hello Timer from Navy-apps @ %d sec %d usec\n", (int)tv.tv_sec, (int)tv.tv_usec);
			//gettimeofday(&tv, NULL);
			last_tv = tv;
    }
  }
  return 0;
}
