#include "common.h"
#include "am.h"

static _RegSet* do_event(_Event e, _RegSet* r) {
  extern _RegSet* do_syscall(_RegSet *r);
  switch (e.event) {
    case _EVENT_SYSCALL:return do_syscall(r);
    case _EVENT_TRAP:
      Log("reach _EVENT_TRAP in do_event");
      return NULL;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
