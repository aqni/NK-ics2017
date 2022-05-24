#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  long long temp=(long long)a*(long long)b;
  int high32=temp>>32;
  unsigned low32=temp;
  return (high32<<16)|(low32>>16);
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  int high32=a >> 16;
  unsigned low32 =  ((unsigned)a) << 16;
  FLOAT result;
  asm volatile (
    "idiv %2" 
    : "=a"(result)
    : "r"(b), "a"(low32), "d"(high32)
  );
  return result;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */
  struct{
    uint32_t fraction:23;
    uint32_t exponent:8;
    uint32_t sign:1;
  } *pfl=&a;
  
  int fraction=pfl->fraction;

  if(pfl->exponent==0) 
    return 0;
  else if (pfl->exponent==0xFF) 
    return pfl->sign?-0x7FFFFFFF:0x7FFFFFFF;

  int exponent=pfl->exponent-127-16;
  int temp= (exponent>=0)?((pfl->fraction)<<exponent):((pfl->fraction)>>-exponent);
  return pfl->sign?-temp:temp;

}

FLOAT Fabs(FLOAT a) {
  return a>0?a:-a;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
