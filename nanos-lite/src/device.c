#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

#define MYMIN(a,b) ((a)<(b)?(a):(b))

void dispinfo_read(void *buf, off_t offset, size_t len) {
  int nread=MYMIN(len,sizeof(dispinfo)/sizeof(dispinfo[0])-offset);
  memcpy(buf, dispinfo + offset, nread);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  if(offset<_screen.width*_screen.width){
    int x=offset%_screen.width,y=offset/_screen.width;
    _draw_rect(buf,x,y,len/4,1);
  }
}

void init_device() {
  _ioe_init();

  // DONE: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH: %d\n",  _screen.width);
  sprintf(dispinfo, "HEIGHT: %d\n", _screen.height);
}
