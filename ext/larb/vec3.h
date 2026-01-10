#ifndef VEC3_H
#define VEC3_H

#include "larb.h"

typedef struct {
  double x;
  double y;
  double z;
} Vec3Data;

void Init_vec3(VALUE module);
VALUE vec3_alloc(VALUE klass);
VALUE vec3_initialize(int argc, VALUE *argv, VALUE self);

VALUE vec3_add(VALUE self, VALUE other);
VALUE vec3_sub(VALUE self, VALUE other);
VALUE vec3_mul(VALUE self, VALUE scalar);
VALUE vec3_div(VALUE self, VALUE scalar);
VALUE vec3_negate(VALUE self);
VALUE vec3_dot(VALUE self, VALUE other);
VALUE vec3_cross(VALUE self, VALUE other);
VALUE vec3_length(VALUE self);
VALUE vec3_length_squared(VALUE self);
VALUE vec3_normalize(VALUE self);
VALUE vec3_reflect(VALUE self, VALUE normal);
VALUE vec3_xy(VALUE self);
VALUE vec3_xz(VALUE self);
VALUE vec3_yz(VALUE self);
VALUE vec3_lerp(VALUE self, VALUE other, VALUE t);
VALUE vec3_to_a(VALUE self);
VALUE vec3_to_vec4(int argc, VALUE *argv, VALUE self);
VALUE vec3_aref(VALUE self, VALUE index);
VALUE vec3_equal(VALUE self, VALUE other);
VALUE vec3_near(int argc, VALUE *argv, VALUE self);
VALUE vec3_distance(VALUE self, VALUE other);
VALUE vec3_distance_squared(VALUE self, VALUE other);
VALUE vec3_angle_between(VALUE self, VALUE other);
VALUE vec3_project(VALUE self, VALUE onto);
VALUE vec3_reject(VALUE self, VALUE from);
VALUE vec3_slerp(VALUE self, VALUE other, VALUE t);
VALUE vec3_clamp_length(VALUE self, VALUE max_length);
VALUE vec3_normalize_bang(VALUE self);
VALUE vec3_min(VALUE self, VALUE other);
VALUE vec3_max(VALUE self, VALUE other);
VALUE vec3_abs(VALUE self);
VALUE vec3_floor(VALUE self);
VALUE vec3_ceil(VALUE self);
VALUE vec3_round(VALUE self);
VALUE vec3_inspect(VALUE self);

#endif
