#ifndef MAT3_H
#define MAT3_H

#include "larb.h"

typedef struct {
  double data[9];
} Mat3Data;

void Init_mat3(VALUE module);
VALUE mat3_alloc(VALUE klass);
VALUE mat3_initialize(int argc, VALUE *argv, VALUE self);

VALUE mat3_aref(VALUE self, VALUE index);
VALUE mat3_aset(VALUE self, VALUE index, VALUE value);
VALUE mat3_mul(VALUE self, VALUE other);
VALUE mat3_add(VALUE self, VALUE other);
VALUE mat3_sub(VALUE self, VALUE other);
VALUE mat3_determinant(VALUE self);
VALUE mat3_inverse(VALUE self);
VALUE mat3_transpose(VALUE self);
VALUE mat3_adjoint(VALUE self);
VALUE mat3_frobenius_norm(VALUE self);
VALUE mat3_translate(VALUE self, VALUE x, VALUE y);
VALUE mat3_rotate(VALUE self, VALUE radians);
VALUE mat3_scale(VALUE self, VALUE x, VALUE y);
VALUE mat3_to_a(VALUE self);
VALUE mat3_equal(VALUE self, VALUE other);
VALUE mat3_near(int argc, VALUE *argv, VALUE self);
VALUE mat3_inspect(VALUE self);
VALUE mat3_data(VALUE self);

#endif
