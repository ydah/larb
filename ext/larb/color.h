#ifndef COLOR_H
#define COLOR_H

#include "larb.h"

typedef struct {
  double r;
  double g;
  double b;
  double a;
} ColorData;

void Init_color(VALUE module);
VALUE color_alloc(VALUE klass);
VALUE color_initialize(int argc, VALUE *argv, VALUE self);

VALUE color_class_from_vec4(VALUE klass, VALUE vec4);
VALUE color_class_from_vec3(int argc, VALUE *argv, VALUE klass);

VALUE color_add(VALUE self, VALUE other);
VALUE color_sub(VALUE self, VALUE other);
VALUE color_mul(VALUE self, VALUE scalar);
VALUE color_lerp(VALUE self, VALUE other, VALUE t);
VALUE color_clamp(VALUE self);
VALUE color_to_bytes(VALUE self);
VALUE color_to_hex(VALUE self);
VALUE color_to_vec3(VALUE self);
VALUE color_to_vec4(VALUE self);
VALUE color_to_a(VALUE self);
VALUE color_aref(VALUE self, VALUE index);
VALUE color_equal(VALUE self, VALUE other);
VALUE color_near(int argc, VALUE *argv, VALUE self);
VALUE color_inspect(VALUE self);

#endif
