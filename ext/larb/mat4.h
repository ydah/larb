#ifndef MAT4_H
#define MAT4_H

#include "larb.h"

typedef struct {
  double data[16];
} Mat4Data;

void Init_mat4(VALUE module);
VALUE mat4_alloc(VALUE klass);
VALUE mat4_initialize(int argc, VALUE *argv, VALUE self);

VALUE mat4_aref(VALUE self, VALUE index);
VALUE mat4_aset(VALUE self, VALUE index, VALUE value);
VALUE mat4_mul(VALUE self, VALUE other);
VALUE mat4_transpose(VALUE self);
VALUE mat4_inverse(VALUE self);
VALUE mat4_to_a(VALUE self);
VALUE mat4_determinant(VALUE self);
VALUE mat4_add(VALUE self, VALUE other);
VALUE mat4_sub(VALUE self, VALUE other);
VALUE mat4_equal(VALUE self, VALUE other);
VALUE mat4_near(int argc, VALUE *argv, VALUE self);
VALUE mat4_extract_translation(VALUE self);
VALUE mat4_extract_scale(VALUE self);
VALUE mat4_extract_rotation(VALUE self);
VALUE mat4_inspect(VALUE self);
VALUE mat4_data(VALUE self);

#endif
