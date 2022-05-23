#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

extern int _read_key();
size_t events_read(void *buf, size_t len) {
  // int key_code;
  int keyinput=_read_key();
  if(keyinput == _KEY_NONE){
    // time event
    snprintf(buf, len, "t %d\n", _uptime());
  }else if(keyinput & 0x8000){
    // key down event
    keyinput &= ~0x8000;
    snprintf(buf, len, "kd %s\n", keyname[keyinput]);

    extern void change_game();
    if (keyinput == _KEY_F12)
      change_game();
  }else{
    // key up event
    snprintf(buf, len, "ku %s\n", keyname[keyinput]);
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

#define MYMIN(a,b) ((a)<(b)?(a):(b))

void dispinfo_read(void *buf, off_t offset, size_t len) {
  memcpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  offset /= sizeof(uint32_t);
  len /= sizeof(uint32_t);
  assert(offset < _screen.width * _screen.height);
  int x=offset%_screen.width,y=offset/ _screen.width;
  _draw_rect(buf,x,y,len,1);
}

void init_device() {
  _ioe_init();

  // DONE: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n",  _screen.width, _screen.height);
}
