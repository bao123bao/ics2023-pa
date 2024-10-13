#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

int main() {
  printf("Hello Timer!\n");
  
	struct timeval tv;
	int last_sec = -1;
	while (1) {
//		printf("ready to call getimte\n");
		gettimeofday(&tv, NULL);

    if (tv.tv_sec != last_sec) {
      printf("Hello Timer from Navy-apps @ %d sec %d usec\n", (int)tv.tv_sec, (int)tv.tv_usec);
			last_sec = tv.tv_sec;
    }
  }
  return 0;
}
