#include "larb.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

VALUE mLarb = Qnil;

void Init_larb(void) {
  mLarb = rb_define_module("Larb");
  Init_vec2(mLarb);
  Init_vec3(mLarb);
  Init_vec4(mLarb);
}
