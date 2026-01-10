#ifndef MAT2_H
#define MAT2_H

#include "larb.h"

typedef struct {
  double data[4];
} Mat2Data;

void Init_mat2(VALUE module);
VALUE mat2_alloc(VALUE klass);
VALUE mat2_initialize(int argc, VALUE *argv, VALUE self);

VALUE mat2_aref(VALUE self, VALUE index);
VALUE mat2_aset(VALUE self, VALUE index, VALUE value);
VALUE mat2_mul(VALUE self, VALUE other);
VALUE mat2_add(VALUE self, VALUE other);
VALUE mat2_sub(VALUE self, VALUE other);
VALUE mat2_determinant(VALUE self);
VALUE mat2_inverse(VALUE self);
VALUE mat2_transpose(VALUE self);
VALUE mat2_adjoint(VALUE self);
VALUE mat2_frobenius_norm(VALUE self);
VALUE mat2_to_a(VALUE self);
VALUE mat2_equal(VALUE self, VALUE other);
VALUE mat2_near(int argc, VALUE *argv, VALUE self);
VALUE mat2_inspect(VALUE self);
VALUE mat2_data(VALUE self);

#endif
