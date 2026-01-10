#include "larb.h"
#include "vec2.h"

VALUE mLarb = Qnil;

void Init_larb(void) {
  mLarb = rb_define_module("Larb");
  Init_vec2(mLarb);
}
