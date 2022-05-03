#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename,0,0);
  size_t len=fs_filesz(fd);
  int nread=fs_read(fd,DEFAULT_ENTRY,len);
  assert(nread==len);
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
