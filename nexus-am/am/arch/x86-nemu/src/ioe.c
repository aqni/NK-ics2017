#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  return inl(RTC_PORT) - boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  for (int row = y; row < y + h; row++)
    for (int col = x; col < x + w; col++) 
      fb[col+row*_screen.width] = pixels[(row-y)*w+(col-x)];
}

void _draw_sync() {
}

#define I8042_DATA_PORT 0x60
#define I8042_STATUS_PORT 0x64
#define I8042_STATUS_HASKEY_MASK 0x1
int _read_key() {
  if (inb(I8042_STATUS_PORT) & I8042_STATUS_HASKEY_MASK){
    return inl(I8042_DATA_PORT);
  }else{
    return _KEY_NONE;
  }
}
