#include "common.h"
#include "syscall.h"
#include "arch.h"
#include "fs.h"

extern int mm_brk(uint32_t new_brk);

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none: SYSCALL_RET(r)=1;break;
    case SYS_exit: _halt(a[1]);break;
    case SYS_brk:  SYSCALL_RET(r) = mm_brk(a[1]);break;
    case SYS_write: SYSCALL_RET(r) = fs_write(a[1], (void *)a[2], a[3]); break;
    case SYS_read: SYSCALL_RET(r) = fs_read(a[1], (void *)a[2], a[3]); break;
    case SYS_open: SYSCALL_RET(r) = fs_open((void *)a[1], a[2], a[3]); break;
    case SYS_close: SYSCALL_RET(r) = fs_close(a[1]); break;
    case SYS_lseek: SYSCALL_RET(r) = fs_lseek(a[1], a[2], a[3]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
