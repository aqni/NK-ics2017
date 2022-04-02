#include "nemu.h"
#include <stdlib.h>
#include <time.h>

CPU_state cpu;

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t eip_sample = rand();
  cpu.eip = eip_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(eip_sample == cpu.eip);

  cpu.eflags.reg=0b111101111011101101;
  assert(cpu.eflags.CF == 1);
  assert(cpu.eflags._1 == 0);
  assert(cpu.eflags.PF == 1);
  assert(cpu.eflags._3 == 1);
  assert(cpu.eflags.AF == 0);
  assert(cpu.eflags._5 == 1);
  assert(cpu.eflags.ZF == 1);
  assert(cpu.eflags.SF == 1);
  assert(cpu.eflags.TF == 0);
  assert(cpu.eflags.IF == 1);
  assert(cpu.eflags.DF == 1);
  assert(cpu.eflags.OF == 1);
  assert(cpu.eflags.IOPL == 0b01);
  assert(cpu.eflags.NT == 1);
  assert(cpu.eflags._f == 1);
  assert(cpu.eflags.RF == 1);
  assert(cpu.eflags.VM == 1);
}
