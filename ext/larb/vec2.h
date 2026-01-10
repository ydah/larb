#ifndef VEC2_H
#define VEC2_H

#include "larb.h"

typedef struct {
  double x;
  double y;
} Vec2Data;

void Init_vec2(VALUE module);
VALUE vec2_alloc(VALUE klass);
VALUE vec2_initialize(int argc, VALUE *argv, VALUE self);

VALUE vec2_add(VALUE self, VALUE other);
VALUE vec2_sub(VALUE self, VALUE other);
VALUE vec2_mul(VALUE self, VALUE scalar);
VALUE vec2_div(VALUE self, VALUE scalar);
VALUE vec2_negate(VALUE self);
VALUE vec2_dot(VALUE self, VALUE other);
VALUE vec2_length(VALUE self);
VALUE vec2_length_squared(VALUE self);
VALUE vec2_normalize(VALUE self);
VALUE vec2_normalize_bang(VALUE self);
VALUE vec2_lerp(VALUE self, VALUE other, VALUE t);
VALUE vec2_to_a(VALUE self);
VALUE vec2_aref(VALUE self, VALUE index);
VALUE vec2_aset(VALUE self, VALUE index, VALUE value);
VALUE vec2_equal(VALUE self, VALUE other);
VALUE vec2_near(int argc, VALUE *argv, VALUE self);
VALUE vec2_angle(VALUE self);
VALUE vec2_angle_to(VALUE self, VALUE other);
VALUE vec2_rotate(VALUE self, VALUE radians);
VALUE vec2_distance(VALUE self, VALUE other);
VALUE vec2_distance_squared(VALUE self, VALUE other);
VALUE vec2_cross(VALUE self, VALUE other);
VALUE vec2_perpendicular(VALUE self);
VALUE vec2_reflect(VALUE self, VALUE normal);
VALUE vec2_clamp_length(VALUE self, VALUE max_length);
VALUE vec2_to_vec3(int argc, VALUE *argv, VALUE self);
VALUE vec2_inspect(VALUE self);

#endif
