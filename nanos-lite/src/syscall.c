#include "common.h"
#include "syscall.h"
#include "arch.h"

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none: SYSCALL_RET(r)=1;return r;
    case SYS_exit: _halt(a[0]);return r;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
