#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // DONE: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:  // Set byte if overflow (OF=1)
      rtl_get_OF(&t3);break;   
    case CC_B:  // Set byte if below (CF=1)
      rtl_get_CF(&t3);break;   
    case CC_E:  // Set byte if equal (ZF=1)
      rtl_get_ZF(&t3);break;   
    case CC_BE: // Set byte if below or equal (CF=1) or (ZF=1)
      rtl_get_CF(&t2); rtl_get_ZF(&t3); rtl_or(&t3,&t2,&t3); break;
    case CC_S:  // Set byte if sign (SF=1)
      rtl_get_SF(&t3);break;
    case CC_L:  // Set byte if less (SF≠OF)
      rtl_get_SF(&t2); rtl_get_OF(&t3); rtl_xor(&t3,&t2,&t3); break;
    case CC_LE: // Set byte if less or equal (ZF=1 and SF≠OF)
      rtl_get_SF(&t2); rtl_get_OF(&t3); rtl_xor(&t3,&t2,&t3); 
      rtl_get_ZF(&t2); rtl_or(&t3,&t2,&t3); break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }
  rtl_mv(dest,&t3);

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
