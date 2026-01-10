#include "mat4.h"

#include <math.h>

static void mat4_free(void *ptr) {
  xfree(ptr);
}

static size_t mat4_memsize(const void *ptr) {
  return sizeof(Mat4Data);
}

static const rb_data_type_t mat4_type = {
    "Mat4",
    {0, mat4_free, mat4_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cMat4 = Qnil;
static VALUE cVec3 = Qnil;
static VALUE cVec4 = Qnil;
static VALUE cQuat = Qnil;

static double value_to_double(VALUE value) {
  VALUE coerced = rb_funcall(value, rb_intern("to_f"), 0);
  return NUM2DBL(coerced);
}

static Mat4Data *mat4_get(VALUE obj) {
  Mat4Data *data = NULL;
  TypedData_Get_Struct(obj, Mat4Data, &mat4_type, data);
  return data;
}

static VALUE mat4_build(VALUE klass, const double *values) {
  VALUE obj = mat4_alloc(klass);
  Mat4Data *data = mat4_get(obj);
  for (int i = 0; i < 16; i++) {
    data->data[i] = values[i];
  }
  return obj;
}

static VALUE mat4_build16(VALUE klass, double v0, double v1, double v2,
                          double v3, double v4, double v5, double v6, double v7,
                          double v8, double v9, double v10, double v11,
                          double v12, double v13, double v14, double v15) {
  double values[16] = {v0,  v1,  v2,  v3,  v4,  v5,  v6,  v7,
                       v8,  v9,  v10, v11, v12, v13, v14, v15};
  return mat4_build(klass, values);
}

static void vec3_normalize(double *x, double *y, double *z) {
  double len = sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
  *x /= len;
  *y /= len;
  *z /= len;
}

static void vec3_cross(double ax, double ay, double az, double bx, double by,
                       double bz, double *rx, double *ry, double *rz) {
  *rx = ay * bz - az * by;
  *ry = az * bx - ax * bz;
  *rz = ax * by - ay * bx;
}

VALUE mat4_alloc(VALUE klass) {
  Mat4Data *data = ALLOC(Mat4Data);
  for (int i = 0; i < 16; i++) {
    data->data[i] = 0.0;
  }
  data->data[0] = 1.0;
  data->data[5] = 1.0;
  data->data[10] = 1.0;
  data->data[15] = 1.0;
  return TypedData_Wrap_Struct(klass, &mat4_type, data);
}

VALUE mat4_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE data_arg = Qnil;
  Mat4Data *data = mat4_get(self);

  rb_scan_args(argc, argv, "01", &data_arg);
  if (NIL_P(data_arg)) {
    for (int i = 0; i < 16; i++) {
      data->data[i] = 0.0;
    }
    data->data[0] = 1.0;
    data->data[5] = 1.0;
    data->data[10] = 1.0;
    data->data[15] = 1.0;
    return self;
  }

  VALUE ary = rb_check_array_type(data_arg);
  if (NIL_P(ary)) {
    rb_raise(rb_eTypeError, "expected Array");
  }

  for (int i = 0; i < 16; i++) {
    data->data[i] = value_to_double(rb_ary_entry(ary, i));
  }

  return self;
}

static VALUE mat4_class_identity(VALUE klass) {
  return mat4_alloc(klass);
}

static VALUE mat4_class_zero(VALUE klass) {
  return mat4_build16(klass, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

static VALUE mat4_class_translation(VALUE klass, VALUE x, VALUE y, VALUE z) {
  VALUE m = mat4_class_identity(klass);
  mat4_aset(m, INT2NUM(12), x);
  mat4_aset(m, INT2NUM(13), y);
  mat4_aset(m, INT2NUM(14), z);
  return m;
}

static VALUE mat4_class_scaling(VALUE klass, VALUE x, VALUE y, VALUE z) {
  double sx = value_to_double(x);
  double sy = value_to_double(y);
  double sz = value_to_double(z);
  return mat4_build16(klass, sx, 0.0, 0.0, 0.0, 0.0, sy, 0.0, 0.0, 0.0, 0.0,
                      sz, 0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_rotation_x(VALUE klass, VALUE radians) {
  double r = value_to_double(radians);
  double c = cos(r);
  double s = sin(r);
  return mat4_build16(klass, 1.0, 0.0, 0.0, 0.0, 0.0, c, s, 0.0, 0.0, -s, c,
                      0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_rotation_y(VALUE klass, VALUE radians) {
  double r = value_to_double(radians);
  double c = cos(r);
  double s = sin(r);
  return mat4_build16(klass, c, 0.0, -s, 0.0, 0.0, 1.0, 0.0, 0.0, s, 0.0, c,
                      0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_rotation_z(VALUE klass, VALUE radians) {
  double r = value_to_double(radians);
  double c = cos(r);
  double s = sin(r);
  return mat4_build16(klass, c, s, 0.0, 0.0, -s, c, 0.0, 0.0, 0.0, 0.0, 1.0,
                      0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_rotation(VALUE klass, VALUE axis, VALUE radians) {
  VALUE normalized = rb_funcall(axis, rb_intern("normalize"), 0);
  double x = value_to_double(rb_funcall(normalized, rb_intern("x"), 0));
  double y = value_to_double(rb_funcall(normalized, rb_intern("y"), 0));
  double z = value_to_double(rb_funcall(normalized, rb_intern("z"), 0));
  double r = value_to_double(radians);
  double c = cos(r);
  double s = sin(r);
  double t = 1.0 - c;

  return mat4_build16(klass, t * x * x + c, t * x * y + s * z,
                      t * x * z - s * y, 0.0, t * x * y - s * z,
                      t * y * y + c, t * y * z + s * x, 0.0,
                      t * x * z + s * y, t * y * z - s * x,
                      t * z * z + c, 0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_look_at(VALUE klass, VALUE eye, VALUE target,
                                VALUE up) {
  double ex = value_to_double(rb_funcall(eye, rb_intern("x"), 0));
  double ey = value_to_double(rb_funcall(eye, rb_intern("y"), 0));
  double ez = value_to_double(rb_funcall(eye, rb_intern("z"), 0));
  double tx = value_to_double(rb_funcall(target, rb_intern("x"), 0));
  double ty = value_to_double(rb_funcall(target, rb_intern("y"), 0));
  double tz = value_to_double(rb_funcall(target, rb_intern("z"), 0));
  double ux = value_to_double(rb_funcall(up, rb_intern("x"), 0));
  double uy = value_to_double(rb_funcall(up, rb_intern("y"), 0));
  double uz = value_to_double(rb_funcall(up, rb_intern("z"), 0));

  double fx = tx - ex;
  double fy = ty - ey;
  double fz = tz - ez;
  vec3_normalize(&fx, &fy, &fz);

  double rx, ry, rz;
  vec3_cross(fx, fy, fz, ux, uy, uz, &rx, &ry, &rz);
  vec3_normalize(&rx, &ry, &rz);

  double ux2, uy2, uz2;
  vec3_cross(rx, ry, rz, fx, fy, fz, &ux2, &uy2, &uz2);

  return mat4_build16(klass, rx, ux2, -fx, 0.0, ry, uy2, -fy, 0.0, rz, uz2,
                      -fz, 0.0, -(rx * ex + ry * ey + rz * ez),
                      -(ux2 * ex + uy2 * ey + uz2 * ez),
                      (fx * ex + fy * ey + fz * ez), 1.0);
}

static VALUE mat4_class_perspective(VALUE klass, VALUE fov_y, VALUE aspect,
                                    VALUE near, VALUE far) {
  double f = 1.0 / tan(value_to_double(fov_y) / 2.0);
  double nf = 1.0 / (value_to_double(near) - value_to_double(far));
  double a = value_to_double(aspect);
  double n = value_to_double(near);
  double fr = value_to_double(far);

  return mat4_build16(klass, f / a, 0.0, 0.0, 0.0, 0.0, f, 0.0, 0.0, 0.0, 0.0,
                      (fr + n) * nf, -1.0, 0.0, 0.0, 2.0 * fr * n * nf, 0.0);
}

static VALUE mat4_class_orthographic(VALUE klass, VALUE left, VALUE right,
                                     VALUE bottom, VALUE top, VALUE near,
                                     VALUE far) {
  double rl = 1.0 / (value_to_double(right) - value_to_double(left));
  double tb = 1.0 / (value_to_double(top) - value_to_double(bottom));
  double fn = 1.0 / (value_to_double(far) - value_to_double(near));
  double r = value_to_double(right);
  double l = value_to_double(left);
  double t = value_to_double(top);
  double b = value_to_double(bottom);
  double f = value_to_double(far);
  double n = value_to_double(near);

  return mat4_build16(klass, 2 * rl, 0.0, 0.0, 0.0, 0.0, 2 * tb, 0.0, 0.0,
                      0.0, 0.0, -2 * fn, 0.0, -(r + l) * rl, -(t + b) * tb,
                      -(f + n) * fn, 1.0);
}

static VALUE mat4_class_frustum(VALUE klass, VALUE left, VALUE right,
                                VALUE bottom, VALUE top, VALUE near,
                                VALUE far) {
  double rl = 1.0 / (value_to_double(right) - value_to_double(left));
  double tb = 1.0 / (value_to_double(top) - value_to_double(bottom));
  double nf = 1.0 / (value_to_double(near) - value_to_double(far));
  double r = value_to_double(right);
  double l = value_to_double(left);
  double t = value_to_double(top);
  double b = value_to_double(bottom);
  double n = value_to_double(near);
  double f = value_to_double(far);

  return mat4_build16(klass, 2 * n * rl, 0.0, 0.0, 0.0, 0.0, 2 * n * tb, 0.0,
                      0.0, (r + l) * rl, (t + b) * tb, (f + n) * nf, -1.0, 0.0,
                      0.0, 2 * f * n * nf, 0.0);
}

static VALUE mat4_class_from_quaternion(VALUE klass, VALUE quat) {
  double x = value_to_double(rb_funcall(quat, rb_intern("x"), 0));
  double y = value_to_double(rb_funcall(quat, rb_intern("y"), 0));
  double z = value_to_double(rb_funcall(quat, rb_intern("z"), 0));
  double w = value_to_double(rb_funcall(quat, rb_intern("w"), 0));

  double x2 = x + x;
  double y2 = y + y;
  double z2 = z + z;
  double xx = x * x2;
  double xy = x * y2;
  double xz = x * z2;
  double yy = y * y2;
  double yz = y * z2;
  double zz = z * z2;
  double wx = w * x2;
  double wy = w * y2;
  double wz = w * z2;

  return mat4_build16(klass, 1 - (yy + zz), xy + wz, xz - wy, 0.0, xy - wz,
                      1 - (xx + zz), yz + wx, 0.0, xz + wy, yz - wx,
                      1 - (xx + yy), 0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_trs(VALUE klass, VALUE translation, VALUE rotation,
                            VALUE scale) {
  VALUE rot = mat4_class_from_quaternion(klass, rotation);
  double sx = value_to_double(rb_funcall(scale, rb_intern("x"), 0));
  double sy = value_to_double(rb_funcall(scale, rb_intern("y"), 0));
  double sz = value_to_double(rb_funcall(scale, rb_intern("z"), 0));
  VALUE scale_m = mat4_class_scaling(klass, DBL2NUM(sx), DBL2NUM(sy), DBL2NUM(sz));
  double tx = value_to_double(rb_funcall(translation, rb_intern("x"), 0));
  double ty = value_to_double(rb_funcall(translation, rb_intern("y"), 0));
  double tz = value_to_double(rb_funcall(translation, rb_intern("z"), 0));
  VALUE trans_m =
      mat4_class_translation(klass, DBL2NUM(tx), DBL2NUM(ty), DBL2NUM(tz));
  VALUE tmp = mat4_mul(rot, scale_m);
  return mat4_mul(tmp, trans_m);
}

VALUE mat4_aref(VALUE self, VALUE index) {
  Mat4Data *data = mat4_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 15) {
    return Qnil;
  }
  return DBL2NUM(data->data[idx]);
}

VALUE mat4_aset(VALUE self, VALUE index, VALUE value) {
  Mat4Data *data = mat4_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 15) {
    rb_raise(rb_eIndexError, "index %ld out of range", idx);
  }
  data->data[idx] = value_to_double(value);
  return value;
}

VALUE mat4_mul(VALUE self, VALUE other) {
  Mat4Data *a = mat4_get(self);

  if (rb_obj_is_kind_of(other, cMat4)) {
    Mat4Data *b = mat4_get(other);
    double result[16];
    for (int i = 0; i < 16; i++) {
      result[i] = 0.0;
    }
    for (int col = 0; col < 4; col++) {
      for (int row = 0; row < 4; row++) {
        for (int k = 0; k < 4; k++) {
          result[col * 4 + row] +=
              a->data[k * 4 + row] * b->data[col * 4 + k];
        }
      }
    }
    return mat4_build(rb_obj_class(self), result);
  }

  if (rb_obj_is_kind_of(other, cVec4)) {
    double x = value_to_double(rb_funcall(other, rb_intern("x"), 0));
    double y = value_to_double(rb_funcall(other, rb_intern("y"), 0));
    double z = value_to_double(rb_funcall(other, rb_intern("z"), 0));
    double w = value_to_double(rb_funcall(other, rb_intern("w"), 0));
    VALUE vec4_class = rb_const_get(mLarb, rb_intern("Vec4"));
    return rb_funcall(
        vec4_class, rb_intern("new"), 4,
        DBL2NUM(a->data[0] * x + a->data[4] * y + a->data[8] * z +
                a->data[12] * w),
        DBL2NUM(a->data[1] * x + a->data[5] * y + a->data[9] * z +
                a->data[13] * w),
        DBL2NUM(a->data[2] * x + a->data[6] * y + a->data[10] * z +
                a->data[14] * w),
        DBL2NUM(a->data[3] * x + a->data[7] * y + a->data[11] * z +
                a->data[15] * w));
  }

  if (rb_obj_is_kind_of(other, cVec3)) {
    VALUE vec4 = rb_funcall(other, rb_intern("to_vec4"), 0);
    return mat4_mul(self, vec4);
  }

  return Qnil;
}

VALUE mat4_transpose(VALUE self) {
  Mat4Data *a = mat4_get(self);
  return mat4_build16(rb_obj_class(self), a->data[0], a->data[4], a->data[8],
                      a->data[12], a->data[1], a->data[5], a->data[9],
                      a->data[13], a->data[2], a->data[6], a->data[10],
                      a->data[14], a->data[3], a->data[7], a->data[11],
                      a->data[15]);
}

VALUE mat4_inverse(VALUE self) {
  Mat4Data *a = mat4_get(self);
  double *m = a->data;
  double inv[16];

  inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] -
           m[9] * m[6] * m[15] + m[9] * m[7] * m[14] +
           m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
  inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] +
           m[8] * m[6] * m[15] - m[8] * m[7] * m[14] -
           m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
  inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] -
           m[8] * m[5] * m[15] + m[8] * m[7] * m[13] +
           m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
  inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] +
            m[8] * m[5] * m[14] - m[8] * m[6] * m[13] -
            m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

  inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] +
           m[9] * m[2] * m[15] - m[9] * m[3] * m[14] -
           m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
  inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] -
           m[8] * m[2] * m[15] + m[8] * m[3] * m[14] +
           m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
  inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] +
           m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -
           m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
  inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] -
            m[8] * m[1] * m[14] + m[8] * m[2] * m[13] +
            m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

  inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] -
           m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +
           m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
  inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] +
           m[4] * m[2] * m[15] - m[4] * m[3] * m[14] -
           m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
  inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] -
            m[4] * m[1] * m[15] + m[4] * m[3] * m[13] +
            m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
  inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] +
            m[4] * m[1] * m[14] - m[4] * m[2] * m[13] -
            m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

  inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] +
           m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
           m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
  inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] -
           m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
           m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
  inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] +
            m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
            m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
  inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] -
            m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
            m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  double det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
  if (fabs(det) < 1e-10) {
    rb_raise(rb_eRuntimeError, "Matrix is not invertible");
  }

  det = 1.0 / det;
  for (int i = 0; i < 16; i++) {
    inv[i] *= det;
  }
  return mat4_build(rb_obj_class(self), inv);
}

VALUE mat4_to_a(VALUE self) {
  Mat4Data *a = mat4_get(self);
  VALUE ary = rb_ary_new_capa(16);
  for (int i = 0; i < 16; i++) {
    rb_ary_push(ary, DBL2NUM(a->data[i]));
  }
  return ary;
}

VALUE mat4_data(VALUE self) {
  return mat4_to_a(self);
}

VALUE mat4_determinant(VALUE self) {
  Mat4Data *a = mat4_get(self);
  double *m = a->data;
  double det =
      m[0] *
          (m[5] * (m[10] * m[15] - m[11] * m[14]) -
           m[9] * (m[6] * m[15] - m[7] * m[14]) +
           m[13] * (m[6] * m[11] - m[7] * m[10])) -
      m[4] *
          (m[1] * (m[10] * m[15] - m[11] * m[14]) -
           m[9] * (m[2] * m[15] - m[3] * m[14]) +
           m[13] * (m[2] * m[11] - m[3] * m[10])) +
      m[8] *
          (m[1] * (m[6] * m[15] - m[7] * m[14]) -
           m[5] * (m[2] * m[15] - m[3] * m[14]) +
           m[13] * (m[2] * m[7] - m[3] * m[6])) -
      m[12] *
          (m[1] * (m[6] * m[11] - m[7] * m[10]) -
           m[5] * (m[2] * m[11] - m[3] * m[10]) +
           m[9] * (m[2] * m[7] - m[3] * m[6]));
  return DBL2NUM(det);
}

VALUE mat4_add(VALUE self, VALUE other) {
  Mat4Data *a = mat4_get(self);
  Mat4Data *b = mat4_get(other);
  double values[16];
  for (int i = 0; i < 16; i++) {
    values[i] = a->data[i] + b->data[i];
  }
  return mat4_build(rb_obj_class(self), values);
}

VALUE mat4_sub(VALUE self, VALUE other) {
  Mat4Data *a = mat4_get(self);
  Mat4Data *b = mat4_get(other);
  double values[16];
  for (int i = 0; i < 16; i++) {
    values[i] = a->data[i] - b->data[i];
  }
  return mat4_build(rb_obj_class(self), values);
}

VALUE mat4_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cMat4)) {
    return Qfalse;
  }
  Mat4Data *a = mat4_get(self);
  Mat4Data *b = mat4_get(other);
  for (int i = 0; i < 16; i++) {
    if (a->data[i] != b->data[i]) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE mat4_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Mat4Data *a = mat4_get(self);
  Mat4Data *b = mat4_get(other);
  double eps = NIL_P(epsilon) ? 1e-6 : value_to_double(epsilon);

  for (int i = 0; i < 16; i++) {
    if (fabs(a->data[i] - b->data[i]) >= eps) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE mat4_extract_translation(VALUE self) {
  Mat4Data *a = mat4_get(self);
  VALUE vec3_class = rb_const_get(mLarb, rb_intern("Vec3"));
  return rb_funcall(vec3_class, rb_intern("new"), 3, DBL2NUM(a->data[12]),
                    DBL2NUM(a->data[13]), DBL2NUM(a->data[14]));
}

VALUE mat4_extract_scale(VALUE self) {
  Mat4Data *a = mat4_get(self);
  double sx = sqrt(a->data[0] * a->data[0] + a->data[1] * a->data[1] +
                   a->data[2] * a->data[2]);
  double sy = sqrt(a->data[4] * a->data[4] + a->data[5] * a->data[5] +
                   a->data[6] * a->data[6]);
  double sz = sqrt(a->data[8] * a->data[8] + a->data[9] * a->data[9] +
                   a->data[10] * a->data[10]);
  VALUE vec3_class = rb_const_get(mLarb, rb_intern("Vec3"));
  return rb_funcall(vec3_class, rb_intern("new"), 3, DBL2NUM(sx), DBL2NUM(sy),
                    DBL2NUM(sz));
}

VALUE mat4_extract_rotation(VALUE self) {
  Mat4Data *a = mat4_get(self);
  VALUE scale = mat4_extract_scale(self);
  double sx = value_to_double(rb_funcall(scale, rb_intern("x"), 0));
  double sy = value_to_double(rb_funcall(scale, rb_intern("y"), 0));
  double sz = value_to_double(rb_funcall(scale, rb_intern("z"), 0));

  double m00 = a->data[0] / sx;
  double m01 = a->data[1] / sx;
  double m02 = a->data[2] / sx;
  double m10 = a->data[4] / sy;
  double m11 = a->data[5] / sy;
  double m12 = a->data[6] / sy;
  double m20 = a->data[8] / sz;
  double m21 = a->data[9] / sz;
  double m22 = a->data[10] / sz;

  double trace = m00 + m11 + m22;
  if (trace > 0.0) {
    double s = 0.5 / sqrt(trace + 1.0);
    return rb_funcall(cQuat, rb_intern("new"), 4,
                      DBL2NUM((m12 - m21) * s),
                      DBL2NUM((m20 - m02) * s),
                      DBL2NUM((m01 - m10) * s), DBL2NUM(0.25 / s));
  }
  if (m00 > m11 && m00 > m22) {
    double s = 2.0 * sqrt(1.0 + m00 - m11 - m22);
    return rb_funcall(cQuat, rb_intern("new"), 4, DBL2NUM(0.25 * s),
                      DBL2NUM((m10 + m01) / s),
                      DBL2NUM((m20 + m02) / s),
                      DBL2NUM((m12 - m21) / s));
  }
  if (m11 > m22) {
    double s = 2.0 * sqrt(1.0 + m11 - m00 - m22);
    return rb_funcall(cQuat, rb_intern("new"), 4,
                      DBL2NUM((m10 + m01) / s), DBL2NUM(0.25 * s),
                      DBL2NUM((m21 + m12) / s),
                      DBL2NUM((m20 - m02) / s));
  }
  double s = 2.0 * sqrt(1.0 + m22 - m00 - m11);
  return rb_funcall(cQuat, rb_intern("new"), 4,
                    DBL2NUM((m20 + m02) / s),
                    DBL2NUM((m21 + m12) / s), DBL2NUM(0.25 * s),
                    DBL2NUM((m01 - m10) / s));
}

static VALUE mat4_format_value(double value) {
  return rb_funcall(rb_mKernel, rb_intern("format"), 2,
                    rb_str_new_cstr("%8.4f"), DBL2NUM(value));
}

static VALUE mat4_row_string(const double *values, int offset) {
  VALUE str = mat4_format_value(values[offset]);
  for (int i = 1; i < 4; i++) {
    rb_str_cat_cstr(str, ", ");
    rb_str_concat(str, mat4_format_value(values[offset + i]));
  }
  return str;
}

VALUE mat4_inspect(VALUE self) {
  Mat4Data *a = mat4_get(self);
  VALUE str = rb_str_new_cstr("Mat4[\n  ");
  rb_str_concat(str, mat4_row_string(a->data, 0));
  rb_str_cat_cstr(str, "\n  ");
  rb_str_concat(str, mat4_row_string(a->data, 4));
  rb_str_cat_cstr(str, "\n  ");
  rb_str_concat(str, mat4_row_string(a->data, 8));
  rb_str_cat_cstr(str, "\n  ");
  rb_str_concat(str, mat4_row_string(a->data, 12));
  rb_str_cat_cstr(str, "\n]");
  return str;
}

void Init_mat4(VALUE module) {
  cMat4 = rb_define_class_under(module, "Mat4", rb_cObject);
  cVec3 = rb_const_get(mLarb, rb_intern("Vec3"));
  cVec4 = rb_const_get(mLarb, rb_intern("Vec4"));
  cQuat = rb_const_get(mLarb, rb_intern("Quat"));

  rb_define_alloc_func(cMat4, mat4_alloc);
  rb_define_method(cMat4, "initialize", mat4_initialize, -1);

  rb_define_singleton_method(cMat4, "identity", mat4_class_identity, 0);
  rb_define_singleton_method(cMat4, "zero", mat4_class_zero, 0);
  rb_define_singleton_method(cMat4, "translation", mat4_class_translation, 3);
  rb_define_singleton_method(cMat4, "scaling", mat4_class_scaling, 3);
  rb_define_singleton_method(cMat4, "rotation_x", mat4_class_rotation_x, 1);
  rb_define_singleton_method(cMat4, "rotation_y", mat4_class_rotation_y, 1);
  rb_define_singleton_method(cMat4, "rotation_z", mat4_class_rotation_z, 1);
  rb_define_singleton_method(cMat4, "rotation", mat4_class_rotation, 2);
  rb_define_singleton_method(cMat4, "look_at", mat4_class_look_at, 3);
  rb_define_singleton_method(cMat4, "perspective", mat4_class_perspective, 4);
  rb_define_singleton_method(cMat4, "orthographic", mat4_class_orthographic,
                             6);
  rb_define_singleton_method(cMat4, "frustum", mat4_class_frustum, 6);
  rb_define_singleton_method(cMat4, "from_quaternion",
                             mat4_class_from_quaternion, 1);
  rb_define_singleton_method(cMat4, "trs", mat4_class_trs, 3);

  rb_define_method(cMat4, "data", mat4_data, 0);
  rb_define_method(cMat4, "[]", mat4_aref, 1);
  rb_define_method(cMat4, "[]=", mat4_aset, 2);
  rb_define_method(cMat4, "*", mat4_mul, 1);
  rb_define_method(cMat4, "transpose", mat4_transpose, 0);
  rb_define_method(cMat4, "inverse", mat4_inverse, 0);
  rb_define_method(cMat4, "to_a", mat4_to_a, 0);
  rb_define_method(cMat4, "determinant", mat4_determinant, 0);
  rb_define_method(cMat4, "+", mat4_add, 1);
  rb_define_method(cMat4, "-", mat4_sub, 1);
  rb_define_method(cMat4, "==", mat4_equal, 1);
  rb_define_method(cMat4, "near?", mat4_near, -1);
  rb_define_method(cMat4, "extract_translation", mat4_extract_translation, 0);
  rb_define_method(cMat4, "extract_scale", mat4_extract_scale, 0);
  rb_define_method(cMat4, "extract_rotation", mat4_extract_rotation, 0);
  rb_define_method(cMat4, "inspect", mat4_inspect, 0);
}
