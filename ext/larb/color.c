#include "color.h"

#include <math.h>

static void color_free(void *ptr) {
  xfree(ptr);
}

static size_t color_memsize(const void *ptr) {
  return sizeof(ColorData);
}

static const rb_data_type_t color_type = {
    "Color",
    {0, color_free, color_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cColor = Qnil;
static VALUE cVec3 = Qnil;
static VALUE cVec4 = Qnil;

static double value_to_double(VALUE value) {
  VALUE coerced = rb_funcall(value, rb_intern("to_f"), 0);
  return NUM2DBL(coerced);
}

static ColorData *color_get(VALUE obj) {
  ColorData *data = NULL;
  TypedData_Get_Struct(obj, ColorData, &color_type, data);
  return data;
}

static VALUE color_build(VALUE klass, double r, double g, double b, double a) {
  VALUE obj = color_alloc(klass);
  ColorData *data = color_get(obj);
  data->r = r;
  data->g = g;
  data->b = b;
  data->a = a;
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

static int clamp_int(int value, int min, int max) {
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}

static VALUE color_get_r(VALUE self) {
  ColorData *data = color_get(self);
  return DBL2NUM(data->r);
}

static VALUE color_set_r(VALUE self, VALUE value) {
  ColorData *data = color_get(self);
  data->r = value_to_double(value);
  return value;
}

static VALUE color_get_g(VALUE self) {
  ColorData *data = color_get(self);
  return DBL2NUM(data->g);
}

static VALUE color_set_g(VALUE self, VALUE value) {
  ColorData *data = color_get(self);
  data->g = value_to_double(value);
  return value;
}

static VALUE color_get_b(VALUE self) {
  ColorData *data = color_get(self);
  return DBL2NUM(data->b);
}

static VALUE color_set_b(VALUE self, VALUE value) {
  ColorData *data = color_get(self);
  data->b = value_to_double(value);
  return value;
}

static VALUE color_get_a(VALUE self) {
  ColorData *data = color_get(self);
  return DBL2NUM(data->a);
}

static VALUE color_set_a(VALUE self, VALUE value) {
  ColorData *data = color_get(self);
  data->a = value_to_double(value);
  return value;
}

VALUE color_alloc(VALUE klass) {
  ColorData *data = ALLOC(ColorData);
  data->r = 0.0;
  data->g = 0.0;
  data->b = 0.0;
  data->a = 1.0;
  return TypedData_Wrap_Struct(klass, &color_type, data);
}

VALUE color_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE vr = Qnil;
  VALUE vg = Qnil;
  VALUE vb = Qnil;
  VALUE va = Qnil;
  ColorData *data = color_get(self);

  rb_scan_args(argc, argv, "04", &vr, &vg, &vb, &va);
  data->r = NIL_P(vr) ? 0.0 : value_to_double(vr);
  data->g = NIL_P(vg) ? 0.0 : value_to_double(vg);
  data->b = NIL_P(vb) ? 0.0 : value_to_double(vb);
  data->a = NIL_P(va) ? 1.0 : value_to_double(va);

  return self;
}

static VALUE color_class_bracket(int argc, VALUE *argv, VALUE klass) {
  VALUE vr = Qnil;
  VALUE vg = Qnil;
  VALUE vb = Qnil;
  VALUE va = Qnil;

  rb_scan_args(argc, argv, "31", &vr, &vg, &vb, &va);
  return color_build(klass, value_to_double(vr), value_to_double(vg),
                     value_to_double(vb), NIL_P(va) ? 1.0 : value_to_double(va));
}

static VALUE color_class_rgb(VALUE klass, VALUE r, VALUE g, VALUE b) {
  return color_build(klass, value_to_double(r), value_to_double(g),
                     value_to_double(b), 1.0);
}

static VALUE color_class_rgba(VALUE klass, VALUE r, VALUE g, VALUE b, VALUE a) {
  return color_build(klass, value_to_double(r), value_to_double(g),
                     value_to_double(b), value_to_double(a));
}

static VALUE color_class_from_bytes(int argc, VALUE *argv, VALUE klass) {
  VALUE vr = Qnil;
  VALUE vg = Qnil;
  VALUE vb = Qnil;
  VALUE va = Qnil;

  rb_scan_args(argc, argv, "31", &vr, &vg, &vb, &va);
  double r = value_to_double(vr) / 255.0;
  double g = value_to_double(vg) / 255.0;
  double b = value_to_double(vb) / 255.0;
  double a = NIL_P(va) ? 1.0 : value_to_double(va) / 255.0;
  return color_build(klass, r, g, b, a);
}

static VALUE color_class_from_hex(VALUE klass, VALUE hex_value) {
  VALUE hex = rb_funcall(hex_value, rb_intern("to_s"), 0);
  VALUE cleaned = rb_funcall(hex, rb_intern("delete"), 1, rb_str_new_cstr("#"));
  VALUE len_val = rb_funcall(cleaned, rb_intern("length"), 0);
  long len = NUM2LONG(len_val);

  VALUE r_str = rb_funcall(cleaned, rb_intern("[]"), 2, INT2NUM(0), INT2NUM(2));
  VALUE g_str = rb_funcall(cleaned, rb_intern("[]"), 2, INT2NUM(2), INT2NUM(2));
  VALUE b_str = rb_funcall(cleaned, rb_intern("[]"), 2, INT2NUM(4), INT2NUM(2));
  double r = value_to_double(rb_funcall(r_str, rb_intern("to_i"), 1,
                                        INT2NUM(16))) /
             255.0;
  double g = value_to_double(rb_funcall(g_str, rb_intern("to_i"), 1,
                                        INT2NUM(16))) /
             255.0;
  double b = value_to_double(rb_funcall(b_str, rb_intern("to_i"), 1,
                                        INT2NUM(16))) /
             255.0;
  double a = 1.0;
  if (len > 6) {
    VALUE a_str =
        rb_funcall(cleaned, rb_intern("[]"), 2, INT2NUM(6), INT2NUM(2));
    a = value_to_double(
            rb_funcall(a_str, rb_intern("to_i"), 1, INT2NUM(16))) /
        255.0;
  }

  return color_build(klass, r, g, b, a);
}

static VALUE color_class_black(VALUE klass) {
  return color_build(klass, 0.0, 0.0, 0.0, 1.0);
}

static VALUE color_class_white(VALUE klass) {
  return color_build(klass, 1.0, 1.0, 1.0, 1.0);
}

static VALUE color_class_red(VALUE klass) {
  return color_build(klass, 1.0, 0.0, 0.0, 1.0);
}

static VALUE color_class_green(VALUE klass) {
  return color_build(klass, 0.0, 1.0, 0.0, 1.0);
}

static VALUE color_class_blue(VALUE klass) {
  return color_build(klass, 0.0, 0.0, 1.0, 1.0);
}

static VALUE color_class_yellow(VALUE klass) {
  return color_build(klass, 1.0, 1.0, 0.0, 1.0);
}

static VALUE color_class_cyan(VALUE klass) {
  return color_build(klass, 0.0, 1.0, 1.0, 1.0);
}

static VALUE color_class_magenta(VALUE klass) {
  return color_build(klass, 1.0, 0.0, 1.0, 1.0);
}

static VALUE color_class_transparent(VALUE klass) {
  return color_build(klass, 0.0, 0.0, 0.0, 0.0);
}

VALUE color_class_from_vec4(VALUE klass, VALUE vec4) {
  double r = value_to_double(rb_funcall(vec4, rb_intern("x"), 0));
  double g = value_to_double(rb_funcall(vec4, rb_intern("y"), 0));
  double b = value_to_double(rb_funcall(vec4, rb_intern("z"), 0));
  double a = value_to_double(rb_funcall(vec4, rb_intern("w"), 0));
  return color_build(klass, r, g, b, a);
}

VALUE color_class_from_vec3(int argc, VALUE *argv, VALUE klass) {
  VALUE vec3 = Qnil;
  VALUE alpha = Qnil;
  rb_scan_args(argc, argv, "11", &vec3, &alpha);
  double r = value_to_double(rb_funcall(vec3, rb_intern("x"), 0));
  double g = value_to_double(rb_funcall(vec3, rb_intern("y"), 0));
  double b = value_to_double(rb_funcall(vec3, rb_intern("z"), 0));
  double a = NIL_P(alpha) ? 1.0 : value_to_double(alpha);
  return color_build(klass, r, g, b, a);
}

VALUE color_add(VALUE self, VALUE other) {
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  return color_build(rb_obj_class(self), a->r + b->r, a->g + b->g, a->b + b->b,
                     a->a + b->a);
}

VALUE color_sub(VALUE self, VALUE other) {
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  return color_build(rb_obj_class(self), a->r - b->r, a->g - b->g, a->b - b->b,
                     a->a - b->a);
}

VALUE color_mul(VALUE self, VALUE scalar) {
  ColorData *a = color_get(self);
  if (rb_obj_is_kind_of(scalar, cColor)) {
    ColorData *b = color_get(scalar);
    return color_build(rb_obj_class(self), a->r * b->r, a->g * b->g,
                       a->b * b->b, a->a * b->a);
  }
  if (rb_obj_is_kind_of(scalar, rb_cNumeric)) {
    double s = value_to_double(scalar);
    return color_build(rb_obj_class(self), a->r * s, a->g * s, a->b * s,
                       a->a * s);
  }
  return Qnil;
}

VALUE color_lerp(VALUE self, VALUE other, VALUE t) {
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  double s = value_to_double(t);
  return color_build(rb_obj_class(self), a->r + (b->r - a->r) * s,
                     a->g + (b->g - a->g) * s, a->b + (b->b - a->b) * s,
                     a->a + (b->a - a->a) * s);
}

VALUE color_clamp(VALUE self) {
  ColorData *a = color_get(self);
  return color_build(rb_obj_class(self), clamp_double(a->r, 0.0, 1.0),
                     clamp_double(a->g, 0.0, 1.0),
                     clamp_double(a->b, 0.0, 1.0),
                     clamp_double(a->a, 0.0, 1.0));
}

VALUE color_to_bytes(VALUE self) {
  ColorData *a = color_get(self);
  int r = (int)lround(a->r * 255.0);
  int g = (int)lround(a->g * 255.0);
  int b = (int)lround(a->b * 255.0);
  int alpha = (int)lround(a->a * 255.0);
  VALUE ary = rb_ary_new_capa(4);
  rb_ary_push(ary, INT2NUM(clamp_int(r, 0, 255)));
  rb_ary_push(ary, INT2NUM(clamp_int(g, 0, 255)));
  rb_ary_push(ary, INT2NUM(clamp_int(b, 0, 255)));
  rb_ary_push(ary, INT2NUM(clamp_int(alpha, 0, 255)));
  return ary;
}

VALUE color_to_hex(VALUE self) {
  VALUE bytes = color_to_bytes(self);
  VALUE r = rb_ary_entry(bytes, 0);
  VALUE g = rb_ary_entry(bytes, 1);
  VALUE b = rb_ary_entry(bytes, 2);
  VALUE a = rb_ary_entry(bytes, 3);
  return rb_funcall(rb_mKernel, rb_intern("format"), 5,
                    rb_str_new_cstr("#%02x%02x%02x%02x"), r, g, b, a);
}

VALUE color_to_vec3(VALUE self) {
  ColorData *a = color_get(self);
  return rb_funcall(cVec3, rb_intern("new"), 3, DBL2NUM(a->r), DBL2NUM(a->g),
                    DBL2NUM(a->b));
}

VALUE color_to_vec4(VALUE self) {
  ColorData *a = color_get(self);
  return rb_funcall(cVec4, rb_intern("new"), 4, DBL2NUM(a->r), DBL2NUM(a->g),
                    DBL2NUM(a->b), DBL2NUM(a->a));
}

VALUE color_to_a(VALUE self) {
  ColorData *a = color_get(self);
  VALUE ary = rb_ary_new_capa(4);
  rb_ary_push(ary, DBL2NUM(a->r));
  rb_ary_push(ary, DBL2NUM(a->g));
  rb_ary_push(ary, DBL2NUM(a->b));
  rb_ary_push(ary, DBL2NUM(a->a));
  return ary;
}

VALUE color_aref(VALUE self, VALUE index) {
  VALUE ary = color_to_a(self);
  return rb_ary_entry(ary, NUM2LONG(index));
}

VALUE color_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cColor)) {
    return Qfalse;
  }
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  return (a->r == b->r && a->g == b->g && a->b == b->b && a->a == b->a)
             ? Qtrue
             : Qfalse;
}

VALUE color_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  double eps = NIL_P(epsilon) ? 1e-6 : value_to_double(epsilon);

  if (fabs(a->r - b->r) < eps && fabs(a->g - b->g) < eps &&
      fabs(a->b - b->b) < eps && fabs(a->a - b->a) < eps) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE color_inspect(VALUE self) {
  ColorData *a = color_get(self);
  VALUE sr = rb_funcall(DBL2NUM(a->r), rb_intern("to_s"), 0);
  VALUE sg = rb_funcall(DBL2NUM(a->g), rb_intern("to_s"), 0);
  VALUE sb = rb_funcall(DBL2NUM(a->b), rb_intern("to_s"), 0);
  VALUE sa = rb_funcall(DBL2NUM(a->a), rb_intern("to_s"), 0);
  VALUE str = rb_str_new_cstr("Color[");
  rb_str_concat(str, sr);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sg);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sb);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sa);
  rb_str_cat_cstr(str, "]");
  return str;
}

void Init_color(VALUE module) {
  cColor = rb_define_class_under(module, "Color", rb_cObject);
  cVec3 = rb_const_get(mLarb, rb_intern("Vec3"));
  cVec4 = rb_const_get(mLarb, rb_intern("Vec4"));

  rb_define_alloc_func(cColor, color_alloc);
  rb_define_method(cColor, "initialize", color_initialize, -1);

  rb_define_singleton_method(cColor, "[]", color_class_bracket, -1);
  rb_define_singleton_method(cColor, "rgb", color_class_rgb, 3);
  rb_define_singleton_method(cColor, "rgba", color_class_rgba, 4);
  rb_define_singleton_method(cColor, "from_bytes", color_class_from_bytes, -1);
  rb_define_singleton_method(cColor, "from_hex", color_class_from_hex, 1);
  rb_define_singleton_method(cColor, "black", color_class_black, 0);
  rb_define_singleton_method(cColor, "white", color_class_white, 0);
  rb_define_singleton_method(cColor, "red", color_class_red, 0);
  rb_define_singleton_method(cColor, "green", color_class_green, 0);
  rb_define_singleton_method(cColor, "blue", color_class_blue, 0);
  rb_define_singleton_method(cColor, "yellow", color_class_yellow, 0);
  rb_define_singleton_method(cColor, "cyan", color_class_cyan, 0);
  rb_define_singleton_method(cColor, "magenta", color_class_magenta, 0);
  rb_define_singleton_method(cColor, "transparent", color_class_transparent, 0);
  rb_define_singleton_method(cColor, "from_vec4", color_class_from_vec4, 1);
  rb_define_singleton_method(cColor, "from_vec3", color_class_from_vec3, -1);

  rb_define_method(cColor, "r", color_get_r, 0);
  rb_define_method(cColor, "r=", color_set_r, 1);
  rb_define_method(cColor, "g", color_get_g, 0);
  rb_define_method(cColor, "g=", color_set_g, 1);
  rb_define_method(cColor, "b", color_get_b, 0);
  rb_define_method(cColor, "b=", color_set_b, 1);
  rb_define_method(cColor, "a", color_get_a, 0);
  rb_define_method(cColor, "a=", color_set_a, 1);

  rb_define_method(cColor, "+", color_add, 1);
  rb_define_method(cColor, "-", color_sub, 1);
  rb_define_method(cColor, "*", color_mul, 1);
  rb_define_method(cColor, "lerp", color_lerp, 2);
  rb_define_method(cColor, "clamp", color_clamp, 0);
  rb_define_method(cColor, "to_bytes", color_to_bytes, 0);
  rb_define_method(cColor, "to_hex", color_to_hex, 0);
  rb_define_method(cColor, "to_vec3", color_to_vec3, 0);
  rb_define_method(cColor, "to_vec4", color_to_vec4, 0);
  rb_define_method(cColor, "to_a", color_to_a, 0);
  rb_define_method(cColor, "[]", color_aref, 1);
  rb_define_method(cColor, "==", color_equal, 1);
  rb_define_method(cColor, "near?", color_near, -1);
  rb_define_method(cColor, "inspect", color_inspect, 0);
  rb_define_alias(cColor, "to_s", "inspect");
}
