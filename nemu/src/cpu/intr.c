#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  vaddr_t desc_addr=cpu.idtr.base+8*NO;
  GateDesc desc;
  *((uint32_t*)&desc)=vaddr_read(desc_addr,4);
  *(((uint32_t*)&desc)+1)=vaddr_read(desc_addr+4,4);
  
  decoding.is_jmp = true;
  decoding.jmp_eip = ((uint32_t)(desc.offset_15_0)) 
    | (((uint32_t)(desc.offset_31_16))<<16);
  
  rtl_push(&cpu.eflags.reg);
  t2=cpu.cs;
  rtl_push(&t2);
  rtl_push(&ret_addr);
 

}

void dev_raise_intr() {
}
