#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <NDL.h>
int main() {
  printf("Hello Timer!\n");
 	NDL_Init(0); 

	uint32_t us, last_us;
	last_us = NDL_GetTicks();
//	struct timeval tv, last_tv;
//	gettimeofday(&last_tv, NULL);
//	printf("start time: %d, %d\n", (int)last_tv.tv_sec, (int)last_tv.tv_usec);
	while (1) {
//		gettimeofday(&tv, NULL);
	//	printf("tv-last = %d\n", tv.tv_usec - last_tv.tv_usec);
 //   if ( (tv.tv_sec-last_tv.tv_sec)*1000000 + (tv.tv_usec - last_tv.tv_usec) >= 500000) {
 		us = NDL_GetTicks();
		if (us - last_us >= 500000) {
  	  printf("Hello Timer from Navy-apps @ %d usec since init\n", (int)us);
			//gettimeofday(&tv, NULL);
			last_us = us;
    }
  }
	NDL_Quit();
  return 0;
}
