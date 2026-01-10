#include "quat2.h"

#include <math.h>

static void quat2_free(void *ptr) {
  xfree(ptr);
}

static size_t quat2_memsize(const void *ptr) {
  return sizeof(Quat2Data);
}

static const rb_data_type_t quat2_type = {
    "Quat2",
    {0, quat2_free, quat2_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cQuat2 = Qnil;
static VALUE cQuat = Qnil;
static VALUE cVec3 = Qnil;
static VALUE cMat4 = Qnil;

static double value_to_double(VALUE value) {
  VALUE coerced = rb_funcall(value, rb_intern("to_f"), 0);
  return NUM2DBL(coerced);
}

static Quat2Data *quat2_get(VALUE obj) {
  Quat2Data *data = NULL;
  TypedData_Get_Struct(obj, Quat2Data, &quat2_type, data);
  return data;
}

static VALUE quat2_build(VALUE klass, const double *values) {
  VALUE obj = quat2_alloc(klass);
  Quat2Data *data = quat2_get(obj);
  for (int i = 0; i < 8; i++) {
    data->data[i] = values[i];
  }
  return obj;
}

static VALUE quat2_build8(VALUE klass, double v0, double v1, double v2,
                          double v3, double v4, double v5, double v6,
                          double v7) {
  double values[8] = {v0, v1, v2, v3, v4, v5, v6, v7};
  return quat2_build(klass, values);
}

static VALUE quat2_class_identity(VALUE klass) {
  return quat2_build8(klass, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
}

static VALUE quat2_class_from_rotation_translation(VALUE klass, VALUE rotation,
                                                   VALUE translation) {
  double rx = value_to_double(rb_funcall(rotation, rb_intern("x"), 0));
  double ry = value_to_double(rb_funcall(rotation, rb_intern("y"), 0));
  double rz = value_to_double(rb_funcall(rotation, rb_intern("z"), 0));
  double rw = value_to_double(rb_funcall(rotation, rb_intern("w"), 0));
  double tx = value_to_double(rb_funcall(translation, rb_intern("x"), 0));
  double ty = value_to_double(rb_funcall(translation, rb_intern("y"), 0));
  double tz = value_to_double(rb_funcall(translation, rb_intern("z"), 0));

  return quat2_build8(
      klass, rx, ry, rz, rw, (tx * rw + ty * rz - tz * ry) * 0.5,
      (ty * rw + tz * rx - tx * rz) * 0.5,
      (tz * rw + tx * ry - ty * rx) * 0.5,
      (-tx * rx - ty * ry - tz * rz) * 0.5);
}

static VALUE quat2_class_from_translation(VALUE klass, VALUE translation) {
  VALUE identity = rb_funcall(cQuat, rb_intern("identity"), 0);
  return quat2_class_from_rotation_translation(klass, identity, translation);
}

static VALUE quat2_class_from_rotation(VALUE klass, VALUE rotation) {
  double rx = value_to_double(rb_funcall(rotation, rb_intern("x"), 0));
  double ry = value_to_double(rb_funcall(rotation, rb_intern("y"), 0));
  double rz = value_to_double(rb_funcall(rotation, rb_intern("z"), 0));
  double rw = value_to_double(rb_funcall(rotation, rb_intern("w"), 0));
  return quat2_build8(klass, rx, ry, rz, rw, 0.0, 0.0, 0.0, 0.0);
}

static VALUE quat2_class_from_mat4(VALUE klass, VALUE mat4) {
  VALUE rotation = rb_funcall(mat4, rb_intern("extract_rotation"), 0);
  VALUE translation = rb_funcall(mat4, rb_intern("extract_translation"), 0);
  return quat2_class_from_rotation_translation(klass, rotation, translation);
}

VALUE quat2_alloc(VALUE klass) {
  Quat2Data *data = ALLOC(Quat2Data);
  data->data[0] = 0.0;
  data->data[1] = 0.0;
  data->data[2] = 0.0;
  data->data[3] = 1.0;
  data->data[4] = 0.0;
  data->data[5] = 0.0;
  data->data[6] = 0.0;
  data->data[7] = 0.0;
  return TypedData_Wrap_Struct(klass, &quat2_type, data);
}

VALUE quat2_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE data_arg = Qnil;
  Quat2Data *data = quat2_get(self);

  rb_scan_args(argc, argv, "01", &data_arg);
  if (NIL_P(data_arg)) {
    data->data[0] = 0.0;
    data->data[1] = 0.0;
    data->data[2] = 0.0;
    data->data[3] = 1.0;
    data->data[4] = 0.0;
    data->data[5] = 0.0;
    data->data[6] = 0.0;
    data->data[7] = 0.0;
    return self;
  }

  VALUE ary = rb_check_array_type(data_arg);
  if (NIL_P(ary)) {
    rb_raise(rb_eTypeError, "expected Array");
  }

  for (int i = 0; i < 8; i++) {
    data->data[i] = value_to_double(rb_ary_entry(ary, i));
  }

  return self;
}

VALUE quat2_real(VALUE self) {
  Quat2Data *a = quat2_get(self);
  return rb_funcall(cQuat, rb_intern("new"), 4, DBL2NUM(a->data[0]),
                    DBL2NUM(a->data[1]), DBL2NUM(a->data[2]),
                    DBL2NUM(a->data[3]));
}

VALUE quat2_dual(VALUE self) {
  Quat2Data *a = quat2_get(self);
  return rb_funcall(cQuat, rb_intern("new"), 4, DBL2NUM(a->data[4]),
                    DBL2NUM(a->data[5]), DBL2NUM(a->data[6]),
                    DBL2NUM(a->data[7]));
}

VALUE quat2_aref(VALUE self, VALUE index) {
  Quat2Data *data = quat2_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 7) {
    return Qnil;
  }
  return DBL2NUM(data->data[idx]);
}

VALUE quat2_aset(VALUE self, VALUE index, VALUE value) {
  Quat2Data *data = quat2_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 7) {
    rb_raise(rb_eIndexError, "index %ld out of range", idx);
  }
  data->data[idx] = value_to_double(value);
  return value;
}

VALUE quat2_mul(VALUE self, VALUE other) {
  Quat2Data *a = quat2_get(self);

  if (rb_obj_is_kind_of(other, cQuat2)) {
    Quat2Data *b = quat2_get(other);
    double ax0 = a->data[0];
    double ay0 = a->data[1];
    double az0 = a->data[2];
    double aw0 = a->data[3];
    double bx1 = a->data[4];
    double by1 = a->data[5];
    double bz1 = a->data[6];
    double bw1 = a->data[7];
    double ax1 = b->data[0];
    double ay1 = b->data[1];
    double az1 = b->data[2];
    double aw1 = b->data[3];
    double bx0 = b->data[4];
    double by0 = b->data[5];
    double bz0 = b->data[6];
    double bw0 = b->data[7];

    return quat2_build8(
        rb_obj_class(self),
        ax0 * aw1 + aw0 * ax1 + ay0 * az1 - az0 * ay1,
        ay0 * aw1 + aw0 * ay1 + az0 * ax1 - ax0 * az1,
        az0 * aw1 + aw0 * az1 + ax0 * ay1 - ay0 * ax1,
        aw0 * aw1 - ax0 * ax1 - ay0 * ay1 - az0 * az1,
        ax0 * bw0 + aw0 * bx0 + ay0 * bz0 - az0 * by0 +
            bx1 * aw1 + bw1 * ax1 + by1 * az1 - bz1 * ay1,
        ay0 * bw0 + aw0 * by0 + az0 * bx0 - ax0 * bz0 +
            by1 * aw1 + bw1 * ay1 + bz1 * ax1 - bx1 * az1,
        az0 * bw0 + aw0 * bz0 + ax0 * by0 - ay0 * bx0 +
            bz1 * aw1 + bw1 * az1 + bx1 * ay1 - by1 * ax1,
        aw0 * bw0 - ax0 * bx0 - ay0 * by0 - az0 * bz0 +
            bw1 * aw1 - bx1 * ax1 - by1 * ay1 - bz1 * az1);
  }

  if (rb_obj_is_kind_of(other, cVec3)) {
    return quat2_transform_point(self, other);
  }

  if (rb_obj_is_kind_of(other, rb_cNumeric)) {
    double s = value_to_double(other);
    double values[8];
    for (int i = 0; i < 8; i++) {
      values[i] = a->data[i] * s;
    }
    return quat2_build(rb_obj_class(self), values);
  }

  return Qnil;
}

VALUE quat2_add(VALUE self, VALUE other) {
  Quat2Data *a = quat2_get(self);
  Quat2Data *b = quat2_get(other);
  double values[8];
  for (int i = 0; i < 8; i++) {
    values[i] = a->data[i] + b->data[i];
  }
  return quat2_build(rb_obj_class(self), values);
}

VALUE quat2_sub(VALUE self, VALUE other) {
  Quat2Data *a = quat2_get(self);
  Quat2Data *b = quat2_get(other);
  double values[8];
  for (int i = 0; i < 8; i++) {
    values[i] = a->data[i] - b->data[i];
  }
  return quat2_build(rb_obj_class(self), values);
}

VALUE quat2_dot(VALUE self, VALUE other) {
  Quat2Data *a = quat2_get(self);
  Quat2Data *b = quat2_get(other);
  return DBL2NUM(a->data[0] * b->data[0] + a->data[1] * b->data[1] +
                 a->data[2] * b->data[2] + a->data[3] * b->data[3]);
}

VALUE quat2_length_squared(VALUE self) {
  Quat2Data *a = quat2_get(self);
  double sum = a->data[0] * a->data[0] + a->data[1] * a->data[1] +
               a->data[2] * a->data[2] + a->data[3] * a->data[3];
  return DBL2NUM(sum);
}

VALUE quat2_length(VALUE self) {
  double len_sq = NUM2DBL(quat2_length_squared(self));
  return DBL2NUM(sqrt(len_sq));
}

VALUE quat2_normalize(VALUE self) {
  Quat2Data *a = quat2_get(self);
  double len = sqrt(a->data[0] * a->data[0] + a->data[1] * a->data[1] +
                    a->data[2] * a->data[2] + a->data[3] * a->data[3]);
  if (len < 1e-10) {
    double values[8];
    for (int i = 0; i < 8; i++) {
      values[i] = a->data[i];
    }
    return quat2_build(rb_obj_class(self), values);
  }
  double inv_len = 1.0 / len;
  double values[8];
  for (int i = 0; i < 8; i++) {
    values[i] = a->data[i] * inv_len;
  }
  return quat2_build(rb_obj_class(self), values);
}

VALUE quat2_normalize_bang(VALUE self) {
  Quat2Data *a = quat2_get(self);
  double len = sqrt(a->data[0] * a->data[0] + a->data[1] * a->data[1] +
                    a->data[2] * a->data[2] + a->data[3] * a->data[3]);
  if (len < 1e-10) {
    return self;
  }
  double inv_len = 1.0 / len;
  for (int i = 0; i < 8; i++) {
    a->data[i] *= inv_len;
  }
  return self;
}

VALUE quat2_conjugate(VALUE self) {
  Quat2Data *a = quat2_get(self);
  return quat2_build8(rb_obj_class(self), -a->data[0], -a->data[1],
                      -a->data[2], a->data[3], -a->data[4], -a->data[5],
                      -a->data[6], a->data[7]);
}

VALUE quat2_inverse(VALUE self) {
  Quat2Data *a = quat2_get(self);
  double len_sq = a->data[0] * a->data[0] + a->data[1] * a->data[1] +
                  a->data[2] * a->data[2] + a->data[3] * a->data[3];
  if (len_sq < 1e-10) {
    return quat2_conjugate(self);
  }
  VALUE conj = quat2_conjugate(self);
  Quat2Data *c = quat2_get(conj);
  double inv_len = 1.0 / len_sq;
  double values[8];
  for (int i = 0; i < 8; i++) {
    values[i] = c->data[i] * inv_len;
  }
  return quat2_build(rb_obj_class(self), values);
}

VALUE quat2_translation(VALUE self) {
  Quat2Data *a = quat2_get(self);
  double ax = a->data[0];
  double ay = a->data[1];
  double az = a->data[2];
  double aw = a->data[3];
  double bx = a->data[4];
  double by = a->data[5];
  double bz = a->data[6];
  double bw = a->data[7];

  double tx = 2.0 * (-bw * ax + bx * aw - by * az + bz * ay);
  double ty = 2.0 * (-bw * ay + by * aw - bz * ax + bx * az);
  double tz = 2.0 * (-bw * az + bz * aw - bx * ay + by * ax);

  return rb_funcall(cVec3, rb_intern("new"), 3, DBL2NUM(tx), DBL2NUM(ty),
                    DBL2NUM(tz));
}

VALUE quat2_rotation(VALUE self) {
  VALUE real = quat2_real(self);
  return rb_funcall(real, rb_intern("normalize"), 0);
}

VALUE quat2_transform_point(VALUE self, VALUE point) {
  VALUE rot = quat2_rotation(self);
  VALUE rotated = rb_funcall(rot, rb_intern("*"), 1, point);
  VALUE trans = quat2_translation(self);
  return rb_funcall(rotated, rb_intern("+"), 1, trans);
}

VALUE quat2_lerp(VALUE self, VALUE other, VALUE t) {
  Quat2Data *a = quat2_get(self);
  Quat2Data *b = quat2_get(other);
  double s = value_to_double(t);
  double values[8];
  for (int i = 0; i < 8; i++) {
    values[i] = a->data[i] + (b->data[i] - a->data[i]) * s;
  }
  VALUE out = quat2_build(rb_obj_class(self), values);
  return quat2_normalize(out);
}

VALUE quat2_to_mat4(VALUE self) {
  VALUE rot = quat2_rotation(self);
  VALUE trans = quat2_translation(self);
  VALUE rot_m = rb_funcall(cMat4, rb_intern("from_quaternion"), 1, rot);
  VALUE trans_m =
      rb_funcall(cMat4, rb_intern("translation"), 3,
                 rb_funcall(trans, rb_intern("x"), 0),
                 rb_funcall(trans, rb_intern("y"), 0),
                 rb_funcall(trans, rb_intern("z"), 0));
  return rb_funcall(rot_m, rb_intern("*"), 1, trans_m);
}

VALUE quat2_to_a(VALUE self) {
  Quat2Data *a = quat2_get(self);
  VALUE ary = rb_ary_new_capa(8);
  for (int i = 0; i < 8; i++) {
    rb_ary_push(ary, DBL2NUM(a->data[i]));
  }
  return ary;
}

VALUE quat2_data(VALUE self) {
  return quat2_to_a(self);
}

VALUE quat2_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cQuat2)) {
    return Qfalse;
  }
  Quat2Data *a = quat2_get(self);
  Quat2Data *b = quat2_get(other);
  for (int i = 0; i < 8; i++) {
    if (a->data[i] != b->data[i]) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE quat2_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Quat2Data *a = quat2_get(self);
  Quat2Data *b = quat2_get(other);
  double eps = NIL_P(epsilon) ? 1e-6 : value_to_double(epsilon);

  for (int i = 0; i < 8; i++) {
    if (fabs(a->data[i] - b->data[i]) >= eps) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE quat2_inspect(VALUE self) {
  VALUE real = quat2_real(self);
  VALUE dual = quat2_dual(self);
  VALUE real_str = rb_funcall(real, rb_intern("inspect"), 0);
  VALUE dual_str = rb_funcall(dual, rb_intern("inspect"), 0);
  VALUE str = rb_str_new_cstr("Quat2[real: ");
  rb_str_concat(str, real_str);
  rb_str_cat_cstr(str, ", dual: ");
  rb_str_concat(str, dual_str);
  rb_str_cat_cstr(str, "]");
  return str;
}

void Init_quat2(VALUE module) {
  cQuat2 = rb_define_class_under(module, "Quat2", rb_cObject);
  cQuat = rb_const_get(mLarb, rb_intern("Quat"));
  cVec3 = rb_const_get(mLarb, rb_intern("Vec3"));
  cMat4 = rb_const_get(mLarb, rb_intern("Mat4"));

  rb_define_alloc_func(cQuat2, quat2_alloc);
  rb_define_method(cQuat2, "initialize", quat2_initialize, -1);

  rb_define_singleton_method(cQuat2, "identity", quat2_class_identity, 0);
  rb_define_singleton_method(cQuat2, "from_rotation_translation",
                             quat2_class_from_rotation_translation, 2);
  rb_define_singleton_method(cQuat2, "from_translation",
                             quat2_class_from_translation, 1);
  rb_define_singleton_method(cQuat2, "from_rotation",
                             quat2_class_from_rotation, 1);
  rb_define_singleton_method(cQuat2, "from_mat4", quat2_class_from_mat4, 1);

  rb_define_method(cQuat2, "data", quat2_data, 0);
  rb_define_method(cQuat2, "real", quat2_real, 0);
  rb_define_method(cQuat2, "dual", quat2_dual, 0);
  rb_define_method(cQuat2, "[]", quat2_aref, 1);
  rb_define_method(cQuat2, "[]=", quat2_aset, 2);
  rb_define_method(cQuat2, "*", quat2_mul, 1);
  rb_define_method(cQuat2, "+", quat2_add, 1);
  rb_define_method(cQuat2, "-", quat2_sub, 1);
  rb_define_method(cQuat2, "dot", quat2_dot, 1);
  rb_define_method(cQuat2, "length", quat2_length, 0);
  rb_define_method(cQuat2, "length_squared", quat2_length_squared, 0);
  rb_define_method(cQuat2, "normalize", quat2_normalize, 0);
  rb_define_method(cQuat2, "normalize!", quat2_normalize_bang, 0);
  rb_define_method(cQuat2, "conjugate", quat2_conjugate, 0);
  rb_define_method(cQuat2, "inverse", quat2_inverse, 0);
  rb_define_method(cQuat2, "translation", quat2_translation, 0);
  rb_define_method(cQuat2, "rotation", quat2_rotation, 0);
  rb_define_method(cQuat2, "transform_point", quat2_transform_point, 1);
  rb_define_method(cQuat2, "lerp", quat2_lerp, 2);
  rb_define_method(cQuat2, "to_mat4", quat2_to_mat4, 0);
  rb_define_method(cQuat2, "to_a", quat2_to_a, 0);
  rb_define_method(cQuat2, "==", quat2_equal, 1);
  rb_define_method(cQuat2, "near?", quat2_near, -1);
  rb_define_method(cQuat2, "inspect", quat2_inspect, 0);
  rb_define_alias(cQuat2, "to_s", "inspect");
}
