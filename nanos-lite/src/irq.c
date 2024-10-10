#include <common.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: printf("nanos: yield event\n"); break;
		case EVENT_SYSCALL: printf("nanos: syscall event\n"); break;
		default: panic("Unhandled event ID = %d", e.event);
  }
	c->mepc += 4;
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
