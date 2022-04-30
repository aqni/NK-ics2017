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
    case SYS_none: SYSCALL_RET(r)=1;break;
    case SYS_exit: _halt(a[1]);break;
    case SYS_write:
      if(a[1]==1||a[1]==2)
        for(int i=0;i<a[3];i++)
          _putc(((uint8_t*)a[2])[i]);
      SYSCALL_RET(r)=a[3];
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
