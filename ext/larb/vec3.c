#include "vec3.h"

#include <math.h>

static void vec3_free(void *ptr) {
  xfree(ptr);
}

static size_t vec3_memsize(const void *ptr) {
  return sizeof(Vec3Data);
}

static const rb_data_type_t vec3_type = {
    "Vec3",
    {0, vec3_free, vec3_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cVec3 = Qnil;

static double value_to_double(VALUE value) {
  VALUE coerced = rb_funcall(value, rb_intern("to_f"), 0);
  return NUM2DBL(coerced);
}

static Vec3Data *vec3_get(VALUE obj) {
  Vec3Data *data = NULL;
  TypedData_Get_Struct(obj, Vec3Data, &vec3_type, data);
  return data;
}

static VALUE vec3_build(VALUE klass, double x, double y, double z) {
  VALUE obj = vec3_alloc(klass);
  Vec3Data *data = vec3_get(obj);
  data->x = x;
  data->y = y;
  data->z = z;
  return obj;
}

static double clamp_double(double value, double min, double max) {
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}

VALUE vec3_alloc(VALUE klass) {
  Vec3Data *data = ALLOC(Vec3Data);
  data->x = 0.0;
  data->y = 0.0;
  data->z = 0.0;
  return TypedData_Wrap_Struct(klass, &vec3_type, data);
}

VALUE vec3_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE vx = Qnil;
  VALUE vy = Qnil;
  VALUE vz = Qnil;
  Vec3Data *data = vec3_get(self);

  rb_scan_args(argc, argv, "03", &vx, &vy, &vz);
  data->x = NIL_P(vx) ? 0.0 : value_to_double(vx);
  data->y = NIL_P(vy) ? 0.0 : value_to_double(vy);
  data->z = NIL_P(vz) ? 0.0 : value_to_double(vz);

  return self;
}

static VALUE vec3_class_bracket(VALUE klass, VALUE x, VALUE y, VALUE z) {
  return vec3_build(klass, value_to_double(x), value_to_double(y),
                    value_to_double(z));
}

static VALUE vec3_class_zero(VALUE klass) {
  return vec3_build(klass, 0.0, 0.0, 0.0);
}

static VALUE vec3_class_one(VALUE klass) {
  return vec3_build(klass, 1.0, 1.0, 1.0);
}

static VALUE vec3_class_up(VALUE klass) {
  return vec3_build(klass, 0.0, 1.0, 0.0);
}

static VALUE vec3_class_down(VALUE klass) {
  return vec3_build(klass, 0.0, -1.0, 0.0);
}

static VALUE vec3_class_forward(VALUE klass) {
  return vec3_build(klass, 0.0, 0.0, -1.0);
}

static VALUE vec3_class_back(VALUE klass) {
  return vec3_build(klass, 0.0, 0.0, 1.0);
}

static VALUE vec3_class_right(VALUE klass) {
  return vec3_build(klass, 1.0, 0.0, 0.0);
}

static VALUE vec3_class_left(VALUE klass) {
  return vec3_build(klass, -1.0, 0.0, 0.0);
}

static VALUE vec3_get_x(VALUE self) {
  Vec3Data *data = vec3_get(self);
  return DBL2NUM(data->x);
}

static VALUE vec3_set_x(VALUE self, VALUE value) {
  Vec3Data *data = vec3_get(self);
  data->x = value_to_double(value);
  return value;
}

static VALUE vec3_get_y(VALUE self) {
  Vec3Data *data = vec3_get(self);
  return DBL2NUM(data->y);
}

static VALUE vec3_set_y(VALUE self, VALUE value) {
  Vec3Data *data = vec3_get(self);
  data->y = value_to_double(value);
  return value;
}

static VALUE vec3_get_z(VALUE self) {
  Vec3Data *data = vec3_get(self);
  return DBL2NUM(data->z);
}

static VALUE vec3_set_z(VALUE self, VALUE value) {
  Vec3Data *data = vec3_get(self);
  data->z = value_to_double(value);
  return value;
}

VALUE vec3_add(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), a->x + b->x, a->y + b->y,
                    a->z + b->z);
}

VALUE vec3_sub(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), a->x - b->x, a->y - b->y,
                    a->z - b->z);
}

VALUE vec3_mul(VALUE self, VALUE scalar) {
  Vec3Data *a = vec3_get(self);

  if (rb_obj_is_kind_of(scalar, cVec3)) {
    Vec3Data *b = vec3_get(scalar);
    return vec3_build(rb_obj_class(self), a->x * b->x, a->y * b->y,
                      a->z * b->z);
  }

  double s = value_to_double(scalar);
  return vec3_build(rb_obj_class(self), a->x * s, a->y * s, a->z * s);
}

VALUE vec3_div(VALUE self, VALUE scalar) {
  Vec3Data *a = vec3_get(self);
  double s = value_to_double(scalar);
  return vec3_build(rb_obj_class(self), a->x / s, a->y / s, a->z / s);
}

VALUE vec3_negate(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), -a->x, -a->y, -a->z);
}

VALUE vec3_dot(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return DBL2NUM(a->x * b->x + a->y * b->y + a->z * b->z);
}

VALUE vec3_cross(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), a->y * b->z - a->z * b->y,
                    a->z * b->x - a->x * b->z,
                    a->x * b->y - a->y * b->x);
}

VALUE vec3_length(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return DBL2NUM(sqrt(a->x * a->x + a->y * a->y + a->z * a->z));
}

VALUE vec3_length_squared(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return DBL2NUM(a->x * a->x + a->y * a->y + a->z * a->z);
}

VALUE vec3_normalize(VALUE self) {
  Vec3Data *a = vec3_get(self);
  double len = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
  return vec3_build(rb_obj_class(self), a->x / len, a->y / len, a->z / len);
}

VALUE vec3_reflect(VALUE self, VALUE normal) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *n = vec3_get(normal);
  double dot = a->x * n->x + a->y * n->y + a->z * n->z;
  return vec3_build(rb_obj_class(self), a->x - n->x * (2.0 * dot),
                    a->y - n->y * (2.0 * dot), a->z - n->z * (2.0 * dot));
}

VALUE vec3_xy(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
  return rb_funcall(vec2_class, rb_intern("new"), 2, DBL2NUM(a->x),
                    DBL2NUM(a->y));
}

VALUE vec3_xz(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
  return rb_funcall(vec2_class, rb_intern("new"), 2, DBL2NUM(a->x),
                    DBL2NUM(a->z));
}

VALUE vec3_yz(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
  return rb_funcall(vec2_class, rb_intern("new"), 2, DBL2NUM(a->y),
                    DBL2NUM(a->z));
}

VALUE vec3_lerp(VALUE self, VALUE other, VALUE t) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double s = value_to_double(t);
  return vec3_build(rb_obj_class(self), a->x + (b->x - a->x) * s,
                    a->y + (b->y - a->y) * s,
                    a->z + (b->z - a->z) * s);
}

VALUE vec3_to_a(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE ary = rb_ary_new_capa(3);
  rb_ary_push(ary, DBL2NUM(a->x));
  rb_ary_push(ary, DBL2NUM(a->y));
  rb_ary_push(ary, DBL2NUM(a->z));
  return ary;
}

VALUE vec3_to_vec4(int argc, VALUE *argv, VALUE self) {
  VALUE vw = Qnil;
  Vec3Data *a = vec3_get(self);

  rb_scan_args(argc, argv, "01", &vw);
  VALUE vec4_class = rb_const_get(mLarb, rb_intern("Vec4"));
  return rb_funcall(vec4_class, rb_intern("new"), 4, DBL2NUM(a->x),
                    DBL2NUM(a->y), DBL2NUM(a->z),
                    NIL_P(vw) ? DBL2NUM(1.0) : vw);
}

VALUE vec3_aref(VALUE self, VALUE index) {
  VALUE ary = vec3_to_a(self);
  return rb_ary_entry(ary, NUM2LONG(index));
}

VALUE vec3_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cVec3)) {
    return Qfalse;
  }
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return (a->x == b->x && a->y == b->y && a->z == b->z) ? Qtrue : Qfalse;
}

VALUE vec3_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double eps = NIL_P(epsilon) ? 1e-6 : value_to_double(epsilon);

  if (fabs(a->x - b->x) < eps && fabs(a->y - b->y) < eps &&
      fabs(a->z - b->z) < eps) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE vec3_distance(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  double dz = a->z - b->z;
  return DBL2NUM(sqrt(dx * dx + dy * dy + dz * dz));
}

VALUE vec3_distance_squared(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  double dz = a->z - b->z;
  return DBL2NUM(dx * dx + dy * dy + dz * dz);
}

VALUE vec3_angle_between(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double len = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
  double other_len = sqrt(b->x * b->x + b->y * b->y + b->z * b->z);
  double dot = a->x * b->x + a->y * b->y + a->z * b->z;
  double d = dot / (len * other_len);
  return DBL2NUM(acos(clamp_double(d, -1.0, 1.0)));
}

VALUE vec3_project(VALUE self, VALUE onto) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(onto);
  double denom = b->x * b->x + b->y * b->y + b->z * b->z;
  double scale = (a->x * b->x + a->y * b->y + a->z * b->z) / denom;
  return vec3_build(rb_obj_class(self), b->x * scale, b->y * scale,
                    b->z * scale);
}

VALUE vec3_reject(VALUE self, VALUE from) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(from);
  double denom = b->x * b->x + b->y * b->y + b->z * b->z;
  double scale = (a->x * b->x + a->y * b->y + a->z * b->z) / denom;
  double px = b->x * scale;
  double py = b->y * scale;
  double pz = b->z * scale;
  return vec3_build(rb_obj_class(self), a->x - px, a->y - py, a->z - pz);
}

VALUE vec3_slerp(VALUE self, VALUE other, VALUE t) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double s = value_to_double(t);

  double len = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
  double other_len = sqrt(b->x * b->x + b->y * b->y + b->z * b->z);
  double dot = (a->x * b->x + a->y * b->y + a->z * b->z) / (len * other_len);
  dot = clamp_double(dot, -1.0, 1.0);

  double theta = acos(dot) * s;
  double rx = b->x - a->x * dot;
  double ry = b->y - a->y * dot;
  double rz = b->z - a->z * dot;
  double rlen = sqrt(rx * rx + ry * ry + rz * rz);
  rx /= rlen;
  ry /= rlen;
  rz /= rlen;

  return vec3_build(rb_obj_class(self), a->x * cos(theta) + rx * sin(theta),
                    a->y * cos(theta) + ry * sin(theta),
                    a->z * cos(theta) + rz * sin(theta));
}

VALUE vec3_clamp_length(VALUE self, VALUE max_length) {
  Vec3Data *a = vec3_get(self);
  double max_len = value_to_double(max_length);
  double len_sq = a->x * a->x + a->y * a->y + a->z * a->z;
  if (len_sq <= max_len * max_len) {
    return self;
  }
  double scale = max_len / sqrt(len_sq);
  return vec3_build(rb_obj_class(self), a->x * scale, a->y * scale,
                    a->z * scale);
}

VALUE vec3_normalize_bang(VALUE self) {
  Vec3Data *a = vec3_get(self);
  double len = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
  a->x /= len;
  a->y /= len;
  a->z /= len;
  return self;
}

VALUE vec3_min(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), fmin(a->x, b->x), fmin(a->y, b->y),
                    fmin(a->z, b->z));
}

VALUE vec3_max(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), fmax(a->x, b->x), fmax(a->y, b->y),
                    fmax(a->z, b->z));
}

VALUE vec3_abs(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), fabs(a->x), fabs(a->y), fabs(a->z));
}

VALUE vec3_floor(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), floor(a->x), floor(a->y), floor(a->z));
}

VALUE vec3_ceil(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), ceil(a->x), ceil(a->y), ceil(a->z));
}

VALUE vec3_round(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), round(a->x), round(a->y), round(a->z));
}

VALUE vec3_inspect(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE sx = rb_funcall(DBL2NUM(a->x), rb_intern("to_s"), 0);
  VALUE sy = rb_funcall(DBL2NUM(a->y), rb_intern("to_s"), 0);
  VALUE sz = rb_funcall(DBL2NUM(a->z), rb_intern("to_s"), 0);
  VALUE str = rb_str_new_cstr("Vec3[");
  rb_str_concat(str, sx);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sy);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sz);
  rb_str_cat_cstr(str, "]");
  return str;
}

void Init_vec3(VALUE module) {
  cVec3 = rb_define_class_under(module, "Vec3", rb_cObject);

  rb_define_alloc_func(cVec3, vec3_alloc);
  rb_define_method(cVec3, "initialize", vec3_initialize, -1);

  rb_define_singleton_method(cVec3, "[]", vec3_class_bracket, 3);
  rb_define_singleton_method(cVec3, "zero", vec3_class_zero, 0);
  rb_define_singleton_method(cVec3, "one", vec3_class_one, 0);
  rb_define_singleton_method(cVec3, "up", vec3_class_up, 0);
  rb_define_singleton_method(cVec3, "down", vec3_class_down, 0);
  rb_define_singleton_method(cVec3, "forward", vec3_class_forward, 0);
  rb_define_singleton_method(cVec3, "back", vec3_class_back, 0);
  rb_define_singleton_method(cVec3, "right", vec3_class_right, 0);
  rb_define_singleton_method(cVec3, "left", vec3_class_left, 0);

  rb_define_method(cVec3, "x", vec3_get_x, 0);
  rb_define_method(cVec3, "x=", vec3_set_x, 1);
  rb_define_method(cVec3, "y", vec3_get_y, 0);
  rb_define_method(cVec3, "y=", vec3_set_y, 1);
  rb_define_method(cVec3, "z", vec3_get_z, 0);
  rb_define_method(cVec3, "z=", vec3_set_z, 1);

  rb_define_method(cVec3, "+", vec3_add, 1);
  rb_define_method(cVec3, "-", vec3_sub, 1);
  rb_define_method(cVec3, "*", vec3_mul, 1);
  rb_define_method(cVec3, "/", vec3_div, 1);
  rb_define_method(cVec3, "-@", vec3_negate, 0);

  rb_define_method(cVec3, "dot", vec3_dot, 1);
  rb_define_method(cVec3, "cross", vec3_cross, 1);
  rb_define_method(cVec3, "length", vec3_length, 0);
  rb_define_method(cVec3, "length_squared", vec3_length_squared, 0);
  rb_define_method(cVec3, "normalize", vec3_normalize, 0);
  rb_define_method(cVec3, "reflect", vec3_reflect, 1);
  rb_define_method(cVec3, "xy", vec3_xy, 0);
  rb_define_method(cVec3, "xz", vec3_xz, 0);
  rb_define_method(cVec3, "yz", vec3_yz, 0);
  rb_define_method(cVec3, "lerp", vec3_lerp, 2);
  rb_define_method(cVec3, "to_a", vec3_to_a, 0);
  rb_define_method(cVec3, "to_vec4", vec3_to_vec4, -1);
  rb_define_method(cVec3, "[]", vec3_aref, 1);
  rb_define_method(cVec3, "==", vec3_equal, 1);
  rb_define_method(cVec3, "near?", vec3_near, -1);
  rb_define_method(cVec3, "distance", vec3_distance, 1);
  rb_define_method(cVec3, "distance_squared", vec3_distance_squared, 1);
  rb_define_method(cVec3, "angle_between", vec3_angle_between, 1);
  rb_define_method(cVec3, "project", vec3_project, 1);
  rb_define_method(cVec3, "reject", vec3_reject, 1);
  rb_define_method(cVec3, "slerp", vec3_slerp, 2);
  rb_define_method(cVec3, "clamp_length", vec3_clamp_length, 1);
  rb_define_method(cVec3, "normalize!", vec3_normalize_bang, 0);
  rb_define_method(cVec3, "min", vec3_min, 1);
  rb_define_method(cVec3, "max", vec3_max, 1);
  rb_define_method(cVec3, "abs", vec3_abs, 0);
  rb_define_method(cVec3, "floor", vec3_floor, 0);
  rb_define_method(cVec3, "ceil", vec3_ceil, 0);
  rb_define_method(cVec3, "round", vec3_round, 0);
  rb_define_method(cVec3, "inspect", vec3_inspect, 0);
  rb_define_alias(cVec3, "to_s", "inspect");
}
