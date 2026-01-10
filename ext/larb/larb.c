#include "larb.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat2.h"
#include "mat3.h"
#include "mat4.h"
#include "quat.h"
#include "mat2d.h"

VALUE mLarb = Qnil;

void Init_larb(void) {
  mLarb = rb_define_module("Larb");
  Init_vec2(mLarb);
  Init_vec3(mLarb);
  Init_vec4(mLarb);
  Init_mat2(mLarb);
  Init_mat3(mLarb);
  Init_quat(mLarb);
  Init_mat4(mLarb);
  Init_mat2d(mLarb);
}
