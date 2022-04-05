#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  extern size_t get_ramdisk_size();
  extern size_t ramdisk_read(void*,off_t,size_t);
  size_t len=get_ramdisk_size();
  ramdisk_read(DEFAULT_ENTRY,0,len); 
  return (uintptr_t)DEFAULT_ENTRY;
}
