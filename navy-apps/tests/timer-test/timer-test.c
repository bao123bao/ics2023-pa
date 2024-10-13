#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

int main() {
  printf("Hello Timer!\n");
  
	struct timeval tv, last_tv;
	last_tv.tv_usec = 0;
	while (1) {
		gettimeofday(&tv, NULL);

    if (tv.tv_usec - last_tv.tv_usec >= 500000) {
      printf("Hello Timer from Navy-apps @ %d sec %d usec\n", (int)tv.tv_sec, (int)tv.tv_usec);
			last_tv = tv;
    }
  }
  return 0;
}
