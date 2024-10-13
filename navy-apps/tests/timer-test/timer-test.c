#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

int main() {
  printf("Hello Timer!\n");
  
	struct timeval tv;

	while (1) {
//		printf("ready to call getimte\n");
		gettimeofday(&tv, NULL);

    if (tv.tv_usec % 500000 == 0) {
      printf("Hello Timer from Navy-apps @ %d sec %d usec\n", (int)tv.tv_sec, (int)tv.tv_usec);
    }
  }
  return 0;
}
