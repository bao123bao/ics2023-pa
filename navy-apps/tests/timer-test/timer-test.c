#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

int main() {
  printf("Hello Timer!\n");
  
	struct timeval tv, last_tv;
	gettimeofday(&last_tv, NULL);
	while (1) {
		gettimeofday(&tv, NULL);
		printf("tv-last = %d\n", tv.tv_usec - last_tv.tv_usec);
    if (tv.tv_usec - last_tv.tv_usec >= 500000) {
      printf("Hello Timer from Navy-apps @ %d sec %d usec\n", (int)tv.tv_sec, (int)tv.tv_usec);
			last_tv = tv;
    }
  }
  return 0;
}
