#include "common.h"
#include "fs.h"
#include "memory.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename,0,0);
  size_t len=fs_filesz(fd);
  uint8_t *va = DEFAULT_ENTRY;

  while(len>0){
    void *pa =new_page();
    _map(as,va,pa);
    int read_len= len > PGSIZE ? PGSIZE:len;
    read_len=fs_read(fd,pa,read_len);
    va += read_len;
    len -= read_len;
  }

  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
