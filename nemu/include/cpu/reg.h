#ifndef __REG_H__
#define __REG_H__

#include "common.h"
#include "memory/mmu.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* DONE: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  union{
    /* TODO: How does it work on big-endian machine? */
    union {
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    } gpr[8];

    /* Do NOT change the order of the GPRs' definitions. */

    /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
     * in PA2 able to directly access these registers.
     */
    struct{
      rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    };
  };

  vaddr_t eip;

  union /* eflags */
  {
    /* Status Flags' Functions

        Bit   Name   Function
        0     CF     Carry Flag ── Set on high-order bit carry or borrow; 
                     cleared otherwise.
        2     PF     Parity Flag ── Set if low-order eight bits of result 
                     contain an even number of 1 bits; cleared otherwise.
        4     AF     Adjust flag ── Set on carry from or borrow to the low 
                     order four bits of AL; cleared otherwise. Used for decimal
                     arithmetic.
        6     ZF     Zero Flag ── Set if result is zero; cleared otherwise.
        7     SF     Sign Flag ── Set equal to high-order bit of result (0 is
                     positive, 1 if negative).
        11    OF     Overflow Flag ── Set if result is too large a positive 
                     number or too small a negative number (excluding sign-bit) 
                     to fit in destination operand; cleared otherwise.
    */
    rtlreg_t reg;
    struct { /* Bit-Fields: https://www.cnblogs.com/axjlxy/p/15008070.html */
      rtlreg_t CF:1, _1:1, PF:1, _3:1, AF:1, _5:1, ZF:1, SF:1, /* 1 - 7 */
               TF:1, IF:1, DF:1, OF:1, IOPL:2,     NT:1, _f:1, /* 8 - 15 */
               RF:1, VM:1, :0;                                 /* 16 -17 */
    };
  } eflags;

  struct {
    uint32_t base;
    uint16_t limit;
  } idtr;

  uint16_t cs;

  CR0 cr0;
  CR3 cr3;

} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

static inline const char* reg_name(int index, int width) {
  assert(index >= 0 && index < 8);
  switch (width) {
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

#endif
