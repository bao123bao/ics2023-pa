#include <unistd.h>
#include <stdio.h>
#include <fixedptc.h>

int main() {
  fixedpt a = fixedpt_rconst(1.2);
	fixedpt b = fixedpt_fromint(10);
	int c = 0;
	//if (b > fixedpt_rconst(7.9)) {
		//c = fixedpt_toint(fixedpt_div(fixedpt_mul(a + FIXEDPT_ONE, b), 		 fixedpt_rconst(2.3)));
	//}
	c = fixedpt_mul(a, b);
	printf("c=0x%x\n", c);
  return 0;
}
