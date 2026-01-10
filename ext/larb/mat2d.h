#ifndef MAT2D_H
#define MAT2D_H

#include "larb.h"

typedef struct {
  double data[6];
} Mat2dData;

void Init_mat2d(VALUE module);
VALUE mat2d_alloc(VALUE klass);
VALUE mat2d_initialize(int argc, VALUE *argv, VALUE self);

VALUE mat2d_aref(VALUE self, VALUE index);
VALUE mat2d_aset(VALUE self, VALUE index, VALUE value);
VALUE mat2d_mul(VALUE self, VALUE other);
VALUE mat2d_add(VALUE self, VALUE other);
VALUE mat2d_sub(VALUE self, VALUE other);
VALUE mat2d_determinant(VALUE self);
VALUE mat2d_inverse(VALUE self);
VALUE mat2d_translate(VALUE self, VALUE x, VALUE y);
VALUE mat2d_rotate(VALUE self, VALUE radians);
VALUE mat2d_scale(VALUE self, VALUE x, VALUE y);
VALUE mat2d_extract_translation(VALUE self);
VALUE mat2d_extract_rotation(VALUE self);
VALUE mat2d_extract_scale(VALUE self);
VALUE mat2d_frobenius_norm(VALUE self);
VALUE mat2d_to_mat3(VALUE self);
VALUE mat2d_to_a(VALUE self);
VALUE mat2d_equal(VALUE self, VALUE other);
VALUE mat2d_near(int argc, VALUE *argv, VALUE self);
VALUE mat2d_inspect(VALUE self);
VALUE mat2d_data(VALUE self);

#endif
