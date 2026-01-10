#include "vec4.h"

#include <math.h>

static void vec4_free(void *ptr) {
  xfree(ptr);
}

static size_t vec4_memsize(const void *ptr) {
  return sizeof(Vec4Data);
}

static const rb_data_type_t vec4_type = {
    "Vec4",
    {0, vec4_free, vec4_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cVec4 = Qnil;

static double value_to_double(VALUE value) {
  VALUE coerced = rb_funcall(value, rb_intern("to_f"), 0);
  return NUM2DBL(coerced);
}

static Vec4Data *vec4_get(VALUE obj) {
  Vec4Data *data = NULL;
  TypedData_Get_Struct(obj, Vec4Data, &vec4_type, data);
  return data;
}

static VALUE vec4_build(VALUE klass, double x, double y, double z, double w) {
  VALUE obj = vec4_alloc(klass);
  Vec4Data *data = vec4_get(obj);
  data->x = x;
  data->y = y;
  data->z = z;
  data->w = w;
  return obj;
}

VALUE vec4_alloc(VALUE klass) {
  Vec4Data *data = ALLOC(Vec4Data);
  data->x = 0.0;
  data->y = 0.0;
  data->z = 0.0;
  data->w = 1.0;
  return TypedData_Wrap_Struct(klass, &vec4_type, data);
}

VALUE vec4_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE vx = Qnil;
  VALUE vy = Qnil;
  VALUE vz = Qnil;
  VALUE vw = Qnil;
  Vec4Data *data = vec4_get(self);

  rb_scan_args(argc, argv, "04", &vx, &vy, &vz, &vw);
  data->x = NIL_P(vx) ? 0.0 : value_to_double(vx);
  data->y = NIL_P(vy) ? 0.0 : value_to_double(vy);
  data->z = NIL_P(vz) ? 0.0 : value_to_double(vz);
  data->w = NIL_P(vw) ? 1.0 : value_to_double(vw);

  return self;
}

static VALUE vec4_class_bracket(int argc, VALUE *argv, VALUE klass) {
  VALUE vx = Qnil;
  VALUE vy = Qnil;
  VALUE vz = Qnil;
  VALUE vw = Qnil;

  rb_scan_args(argc, argv, "31", &vx, &vy, &vz, &vw);
  return vec4_build(klass, value_to_double(vx), value_to_double(vy),
                    value_to_double(vz), NIL_P(vw) ? 1.0 : value_to_double(vw));
}

static VALUE vec4_class_zero(VALUE klass) {
  return vec4_build(klass, 0.0, 0.0, 0.0, 0.0);
}

static VALUE vec4_class_one(VALUE klass) {
  return vec4_build(klass, 1.0, 1.0, 1.0, 1.0);
}

static VALUE vec4_get_x(VALUE self) {
  Vec4Data *data = vec4_get(self);
  return DBL2NUM(data->x);
}

static VALUE vec4_set_x(VALUE self, VALUE value) {
  Vec4Data *data = vec4_get(self);
  data->x = value_to_double(value);
  return value;
}

static VALUE vec4_get_y(VALUE self) {
  Vec4Data *data = vec4_get(self);
  return DBL2NUM(data->y);
}

static VALUE vec4_set_y(VALUE self, VALUE value) {
  Vec4Data *data = vec4_get(self);
  data->y = value_to_double(value);
  return value;
}

static VALUE vec4_get_z(VALUE self) {
  Vec4Data *data = vec4_get(self);
  return DBL2NUM(data->z);
}

static VALUE vec4_set_z(VALUE self, VALUE value) {
  Vec4Data *data = vec4_get(self);
  data->z = value_to_double(value);
  return value;
}

static VALUE vec4_get_w(VALUE self) {
  Vec4Data *data = vec4_get(self);
  return DBL2NUM(data->w);
}

static VALUE vec4_set_w(VALUE self, VALUE value) {
  Vec4Data *data = vec4_get(self);
  data->w = value_to_double(value);
  return value;
}

VALUE vec4_add(VALUE self, VALUE other) {
  Vec4Data *a = vec4_get(self);
  Vec4Data *b = vec4_get(other);
  return vec4_build(rb_obj_class(self), a->x + b->x, a->y + b->y,
                    a->z + b->z, a->w + b->w);
}

VALUE vec4_sub(VALUE self, VALUE other) {
  Vec4Data *a = vec4_get(self);
  Vec4Data *b = vec4_get(other);
  return vec4_build(rb_obj_class(self), a->x - b->x, a->y - b->y,
                    a->z - b->z, a->w - b->w);
}

VALUE vec4_mul(VALUE self, VALUE scalar) {
  Vec4Data *a = vec4_get(self);
  double s = value_to_double(scalar);
  return vec4_build(rb_obj_class(self), a->x * s, a->y * s, a->z * s,
                    a->w * s);
}

VALUE vec4_div(VALUE self, VALUE scalar) {
  Vec4Data *a = vec4_get(self);
  double s = value_to_double(scalar);
  return vec4_build(rb_obj_class(self), a->x / s, a->y / s, a->z / s,
                    a->w / s);
}

VALUE vec4_negate(VALUE self) {
  Vec4Data *a = vec4_get(self);
  return vec4_build(rb_obj_class(self), -a->x, -a->y, -a->z, -a->w);
}

VALUE vec4_dot(VALUE self, VALUE other) {
  Vec4Data *a = vec4_get(self);
  Vec4Data *b = vec4_get(other);
  return DBL2NUM(a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w);
}

VALUE vec4_length(VALUE self) {
  Vec4Data *a = vec4_get(self);
  return DBL2NUM(sqrt(a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w));
}

VALUE vec4_length_squared(VALUE self) {
  Vec4Data *a = vec4_get(self);
  return DBL2NUM(a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w);
}

VALUE vec4_normalize(VALUE self) {
  Vec4Data *a = vec4_get(self);
  double len = sqrt(a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w);
  return vec4_build(rb_obj_class(self), a->x / len, a->y / len, a->z / len,
                    a->w / len);
}

VALUE vec4_normalize_bang(VALUE self) {
  Vec4Data *a = vec4_get(self);
  double len = sqrt(a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w);
  a->x /= len;
  a->y /= len;
  a->z /= len;
  a->w /= len;
  return self;
}

VALUE vec4_perspective_divide(VALUE self) {
  Vec4Data *a = vec4_get(self);
  VALUE vec3_class = rb_const_get(mLarb, rb_intern("Vec3"));

  if (a->w == 0.0 || a->w == 1.0) {
    return rb_funcall(vec3_class, rb_intern("new"), 3, DBL2NUM(a->x),
                      DBL2NUM(a->y), DBL2NUM(a->z));
  }

  return rb_funcall(vec3_class, rb_intern("new"), 3, DBL2NUM(a->x / a->w),
                    DBL2NUM(a->y / a->w), DBL2NUM(a->z / a->w));
}

VALUE vec4_xyz(VALUE self) {
  Vec4Data *a = vec4_get(self);
  VALUE vec3_class = rb_const_get(mLarb, rb_intern("Vec3"));
  return rb_funcall(vec3_class, rb_intern("new"), 3, DBL2NUM(a->x),
                    DBL2NUM(a->y), DBL2NUM(a->z));
}

VALUE vec4_xy(VALUE self) {
  Vec4Data *a = vec4_get(self);
  VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
  return rb_funcall(vec2_class, rb_intern("new"), 2, DBL2NUM(a->x),
                    DBL2NUM(a->y));
}

VALUE vec4_rgb(VALUE self) {
  return vec4_xyz(self);
}

VALUE vec4_to_a(VALUE self) {
  Vec4Data *a = vec4_get(self);
  VALUE ary = rb_ary_new_capa(4);
  rb_ary_push(ary, DBL2NUM(a->x));
  rb_ary_push(ary, DBL2NUM(a->y));
  rb_ary_push(ary, DBL2NUM(a->z));
  rb_ary_push(ary, DBL2NUM(a->w));
  return ary;
}

VALUE vec4_aref(VALUE self, VALUE index) {
  VALUE ary = vec4_to_a(self);
  return rb_ary_entry(ary, NUM2LONG(index));
}

VALUE vec4_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cVec4)) {
    return Qfalse;
  }
  Vec4Data *a = vec4_get(self);
  Vec4Data *b = vec4_get(other);
  return (a->x == b->x && a->y == b->y && a->z == b->z && a->w == b->w)
             ? Qtrue
             : Qfalse;
}

VALUE vec4_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Vec4Data *a = vec4_get(self);
  Vec4Data *b = vec4_get(other);
  double eps = NIL_P(epsilon) ? 1e-6 : value_to_double(epsilon);

  if (fabs(a->x - b->x) < eps && fabs(a->y - b->y) < eps &&
      fabs(a->z - b->z) < eps && fabs(a->w - b->w) < eps) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE vec4_lerp(VALUE self, VALUE other, VALUE t) {
  Vec4Data *a = vec4_get(self);
  Vec4Data *b = vec4_get(other);
  double s = value_to_double(t);
  return vec4_build(rb_obj_class(self), a->x + (b->x - a->x) * s,
                    a->y + (b->y - a->y) * s,
                    a->z + (b->z - a->z) * s,
                    a->w + (b->w - a->w) * s);
}

VALUE vec4_inspect(VALUE self) {
  Vec4Data *a = vec4_get(self);
  VALUE sx = rb_funcall(DBL2NUM(a->x), rb_intern("to_s"), 0);
  VALUE sy = rb_funcall(DBL2NUM(a->y), rb_intern("to_s"), 0);
  VALUE sz = rb_funcall(DBL2NUM(a->z), rb_intern("to_s"), 0);
  VALUE sw = rb_funcall(DBL2NUM(a->w), rb_intern("to_s"), 0);
  VALUE str = rb_str_new_cstr("Vec4[");
  rb_str_concat(str, sx);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sy);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sz);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sw);
  rb_str_cat_cstr(str, "]");
  return str;
}

void Init_vec4(VALUE module) {
  cVec4 = rb_define_class_under(module, "Vec4", rb_cObject);

  rb_define_alloc_func(cVec4, vec4_alloc);
  rb_define_method(cVec4, "initialize", vec4_initialize, -1);

  rb_define_singleton_method(cVec4, "[]", vec4_class_bracket, -1);
  rb_define_singleton_method(cVec4, "zero", vec4_class_zero, 0);
  rb_define_singleton_method(cVec4, "one", vec4_class_one, 0);

  rb_define_method(cVec4, "x", vec4_get_x, 0);
  rb_define_method(cVec4, "x=", vec4_set_x, 1);
  rb_define_method(cVec4, "y", vec4_get_y, 0);
  rb_define_method(cVec4, "y=", vec4_set_y, 1);
  rb_define_method(cVec4, "z", vec4_get_z, 0);
  rb_define_method(cVec4, "z=", vec4_set_z, 1);
  rb_define_method(cVec4, "w", vec4_get_w, 0);
  rb_define_method(cVec4, "w=", vec4_set_w, 1);

  rb_define_method(cVec4, "+", vec4_add, 1);
  rb_define_method(cVec4, "-", vec4_sub, 1);
  rb_define_method(cVec4, "*", vec4_mul, 1);
  rb_define_method(cVec4, "/", vec4_div, 1);
  rb_define_method(cVec4, "-@", vec4_negate, 0);

  rb_define_method(cVec4, "dot", vec4_dot, 1);
  rb_define_method(cVec4, "length", vec4_length, 0);
  rb_define_method(cVec4, "length_squared", vec4_length_squared, 0);
  rb_define_method(cVec4, "normalize", vec4_normalize, 0);
  rb_define_method(cVec4, "normalize!", vec4_normalize_bang, 0);
  rb_define_method(cVec4, "perspective_divide", vec4_perspective_divide, 0);
  rb_define_method(cVec4, "xyz", vec4_xyz, 0);
  rb_define_method(cVec4, "xy", vec4_xy, 0);
  rb_define_method(cVec4, "rgb", vec4_rgb, 0);
  rb_define_method(cVec4, "to_a", vec4_to_a, 0);
  rb_define_method(cVec4, "[]", vec4_aref, 1);
  rb_define_method(cVec4, "==", vec4_equal, 1);
  rb_define_method(cVec4, "near?", vec4_near, -1);
  rb_define_method(cVec4, "lerp", vec4_lerp, 2);
  rb_define_method(cVec4, "inspect", vec4_inspect, 0);
  rb_define_alias(cVec4, "to_s", "inspect");
}
