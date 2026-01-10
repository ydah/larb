#ifndef QUAT_H
#define QUAT_H

#include "larb.h"

typedef struct {
  double x;
  double y;
  double z;
  double w;
} QuatData;

void Init_quat(VALUE module);
VALUE quat_alloc(VALUE klass);
VALUE quat_initialize(int argc, VALUE *argv, VALUE self);

VALUE quat_mul(VALUE self, VALUE other);
VALUE quat_add(VALUE self, VALUE other);
VALUE quat_sub(VALUE self, VALUE other);
VALUE quat_negate(VALUE self);
VALUE quat_dot(VALUE self, VALUE other);
VALUE quat_length(VALUE self);
VALUE quat_length_squared(VALUE self);
VALUE quat_normalize(VALUE self);
VALUE quat_normalize_bang(VALUE self);
VALUE quat_conjugate(VALUE self);
VALUE quat_inverse(VALUE self);
VALUE quat_lerp(VALUE self, VALUE other, VALUE t);
VALUE quat_slerp(VALUE self, VALUE other, VALUE t);
VALUE quat_to_axis_angle(VALUE self);
VALUE quat_to_euler(VALUE self);
VALUE quat_to_mat4(VALUE self);
VALUE quat_to_a(VALUE self);
VALUE quat_aref(VALUE self, VALUE index);
VALUE quat_equal(VALUE self, VALUE other);
VALUE quat_near(int argc, VALUE *argv, VALUE self);
VALUE quat_inspect(VALUE self);

#endif
