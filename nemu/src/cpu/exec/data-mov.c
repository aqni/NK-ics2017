#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  if (decoding.is_operand_size_16) {
    TODO();
  }else{
    rtl_push(&id_dest->val);
  }

  print_asm_template1(push);
}

make_EHelper(pop) {
  if (decoding.is_operand_size_16) {
    TODO();
  }else{
    rtl_pop(&t3);
    operand_write(id_dest, &t3);
  }

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  if (decoding.is_operand_size_16) {
    TODO();
  }else{
    rtl_lr(&t1,R_ESP,4);
    rtl_lr(&t0,R_EAX,4);rtl_push(&t0);
    rtl_lr(&t0,R_ECX,4);rtl_push(&t0);
    rtl_lr(&t0,R_EDX,4);rtl_push(&t0);
    rtl_lr(&t0,R_EBX,4);rtl_push(&t0);
    rtl_push(&t1);
    rtl_lr(&t0,R_EBP,4);rtl_push(&t0);
    rtl_lr(&t0,R_ESI,4);rtl_push(&t0);
    rtl_lr(&t0,R_EDI,4);rtl_push(&t0);
  }

  print_asm("pusha");
}

make_EHelper(popa) {
  if (decoding.is_operand_size_16) {
    TODO();
  }else{
    rtl_pop(&t0);rtl_sr(R_EDI,4,&t0);
    rtl_pop(&t0);rtl_sr(R_ESI,4,&t0);
    rtl_pop(&t0);rtl_sr(R_EBP,4,&t0);
    rtl_pop(&t0);
    rtl_pop(&t0);rtl_sr(R_EBX,4,&t0);
    rtl_pop(&t0);rtl_sr(R_EDX,4,&t0);
    rtl_pop(&t0);rtl_sr(R_ECX,4,&t0);
    rtl_pop(&t0);rtl_sr(R_EAX,4,&t0);
  }

  print_asm("popa");
}

make_EHelper(leave) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    rtl_lr(&t0,R_EBP,4);
    rtl_sr(R_ESP,4,&t0);
    rtl_pop(&t0);
    rtl_sr(R_EBP,4,&t0);
  }

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    rtl_lr(&t2, R_AX,2);
    rtl_sext(&t2, &t2, 2);
    rtl_sari(&t2, &t2, 16);
    rtl_sr(R_DX,2, &t2);
  } else {
    rtl_lr(&t2, R_EAX,4);
    rtl_sari(&t2, &t2, 16); //直接移位32位会出现浮点异常
    rtl_sari(&t2, &t2, 16);
    rtl_sr(R_EDX, 4, &t2);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_lr(&t2, R_AX, 1);
    rtl_sext(&t2, &t2, 1);
    rtl_sr(R_AX,2, &t2);
  }
  else {
    rtl_lr(&t0, R_AX, 2);
    rtl_sext(&t0, &t0, 2);
    rtl_sr(R_EAX, 4, &t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
