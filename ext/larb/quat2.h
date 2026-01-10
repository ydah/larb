#ifndef QUAT2_H
#define QUAT2_H

#include "larb.h"

typedef struct {
  double data[8];
} Quat2Data;

void Init_quat2(VALUE module);
VALUE quat2_alloc(VALUE klass);
VALUE quat2_initialize(int argc, VALUE *argv, VALUE self);

VALUE quat2_real(VALUE self);
VALUE quat2_dual(VALUE self);
VALUE quat2_aref(VALUE self, VALUE index);
VALUE quat2_aset(VALUE self, VALUE index, VALUE value);
VALUE quat2_mul(VALUE self, VALUE other);
VALUE quat2_add(VALUE self, VALUE other);
VALUE quat2_sub(VALUE self, VALUE other);
VALUE quat2_dot(VALUE self, VALUE other);
VALUE quat2_length(VALUE self);
VALUE quat2_length_squared(VALUE self);
VALUE quat2_normalize(VALUE self);
VALUE quat2_normalize_bang(VALUE self);
VALUE quat2_conjugate(VALUE self);
VALUE quat2_inverse(VALUE self);
VALUE quat2_translation(VALUE self);
VALUE quat2_rotation(VALUE self);
VALUE quat2_transform_point(VALUE self, VALUE point);
VALUE quat2_lerp(VALUE self, VALUE other, VALUE t);
VALUE quat2_to_mat4(VALUE self);
VALUE quat2_to_a(VALUE self);
VALUE quat2_equal(VALUE self, VALUE other);
VALUE quat2_near(int argc, VALUE *argv, VALUE self);
VALUE quat2_inspect(VALUE self);
VALUE quat2_data(VALUE self);

#endif
