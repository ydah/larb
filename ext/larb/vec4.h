#ifndef VEC4_H
#define VEC4_H

#include "larb.h"

typedef struct {
  double x;
  double y;
  double z;
  double w;
} Vec4Data;

void Init_vec4(VALUE module);
VALUE vec4_alloc(VALUE klass);
VALUE vec4_initialize(int argc, VALUE *argv, VALUE self);

VALUE vec4_add(VALUE self, VALUE other);
VALUE vec4_sub(VALUE self, VALUE other);
VALUE vec4_mul(VALUE self, VALUE scalar);
VALUE vec4_div(VALUE self, VALUE scalar);
VALUE vec4_negate(VALUE self);
VALUE vec4_dot(VALUE self, VALUE other);
VALUE vec4_length(VALUE self);
VALUE vec4_length_squared(VALUE self);
VALUE vec4_normalize(VALUE self);
VALUE vec4_normalize_bang(VALUE self);
VALUE vec4_perspective_divide(VALUE self);
VALUE vec4_xyz(VALUE self);
VALUE vec4_xy(VALUE self);
VALUE vec4_rgb(VALUE self);
VALUE vec4_to_a(VALUE self);
VALUE vec4_aref(VALUE self, VALUE index);
VALUE vec4_equal(VALUE self, VALUE other);
VALUE vec4_near(int argc, VALUE *argv, VALUE self);
VALUE vec4_lerp(VALUE self, VALUE other, VALUE t);
VALUE vec4_inspect(VALUE self);

#endif
