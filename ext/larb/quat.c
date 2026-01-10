#include "quat.h"

#include <math.h>

static void quat_free(void *ptr) {
  xfree(ptr);
}

static size_t quat_memsize(const void *ptr) {
  return sizeof(QuatData);
}

static const rb_data_type_t quat_type = {
    "Quat",
    {0, quat_free, quat_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cQuat = Qnil;
static VALUE cVec3 = Qnil;

static double value_to_double(VALUE value) {
  VALUE coerced = rb_funcall(value, rb_intern("to_f"), 0);
  return NUM2DBL(coerced);
}

static QuatData *quat_get(VALUE obj) {
  QuatData *data = NULL;
  TypedData_Get_Struct(obj, QuatData, &quat_type, data);
  return data;
}

static VALUE quat_build(VALUE klass, double x, double y, double z, double w) {
  VALUE obj = quat_alloc(klass);
  QuatData *data = quat_get(obj);
  data->x = x;
  data->y = y;
  data->z = z;
  data->w = w;
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

static void normalize_quat(double *x, double *y, double *z, double *w) {
  double len = sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z) + (*w) * (*w));
  *x /= len;
  *y /= len;
  *z /= len;
  *w /= len;
}

VALUE quat_alloc(VALUE klass) {
  QuatData *data = ALLOC(QuatData);
  data->x = 0.0;
  data->y = 0.0;
  data->z = 0.0;
  data->w = 1.0;
  return TypedData_Wrap_Struct(klass, &quat_type, data);
}

VALUE quat_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE vx = Qnil;
  VALUE vy = Qnil;
  VALUE vz = Qnil;
  VALUE vw = Qnil;
  QuatData *data = quat_get(self);

  rb_scan_args(argc, argv, "04", &vx, &vy, &vz, &vw);
  data->x = NIL_P(vx) ? 0.0 : value_to_double(vx);
  data->y = NIL_P(vy) ? 0.0 : value_to_double(vy);
  data->z = NIL_P(vz) ? 0.0 : value_to_double(vz);
  data->w = NIL_P(vw) ? 1.0 : value_to_double(vw);

  return self;
}

static VALUE quat_class_bracket(VALUE klass, VALUE x, VALUE y, VALUE z,
                                VALUE w) {
  return quat_build(klass, value_to_double(x), value_to_double(y),
                    value_to_double(z), value_to_double(w));
}

static VALUE quat_class_identity(VALUE klass) {
  return quat_build(klass, 0.0, 0.0, 0.0, 1.0);
}

static VALUE quat_class_from_axis_angle(VALUE klass, VALUE axis,
                                        VALUE radians) {
  double half = value_to_double(radians) * 0.5;
  double s = sin(half);
  VALUE normalized = rb_funcall(axis, rb_intern("normalize"), 0);
  double x = value_to_double(rb_funcall(normalized, rb_intern("x"), 0));
  double y = value_to_double(rb_funcall(normalized, rb_intern("y"), 0));
  double z = value_to_double(rb_funcall(normalized, rb_intern("z"), 0));
  return quat_build(klass, x * s, y * s, z * s, cos(half));
}

static VALUE quat_class_from_euler(VALUE klass, VALUE x, VALUE y, VALUE z) {
  double rx = value_to_double(x) * 0.5;
  double ry = value_to_double(y) * 0.5;
  double rz = value_to_double(z) * 0.5;
  double cx = cos(rx);
  double sx = sin(rx);
  double cy = cos(ry);
  double sy = sin(ry);
  double cz = cos(rz);
  double sz = sin(rz);

  return quat_build(klass, sx * cy * cz - cx * sy * sz,
                    cx * sy * cz + sx * cy * sz,
                    cx * cy * sz - sx * sy * cz,
                    cx * cy * cz + sx * sy * sz);
}

static VALUE quat_class_look_rotation(int argc, VALUE *argv, VALUE klass) {
  VALUE forward = Qnil;
  VALUE up = Qnil;
  rb_scan_args(argc, argv, "11", &forward, &up);

  VALUE forward_norm = rb_funcall(forward, rb_intern("normalize"), 0);
  VALUE up_vec = NIL_P(up) ? rb_funcall(cVec3, rb_intern("up"), 0) : up;
  VALUE right =
      rb_funcall(rb_funcall(up_vec, rb_intern("cross"), 1, forward_norm),
                 rb_intern("normalize"), 0);
  VALUE up_final =
      rb_funcall(forward_norm, rb_intern("cross"), 1, right);

  double m00 = value_to_double(rb_funcall(right, rb_intern("x"), 0));
  double m01 = value_to_double(rb_funcall(up_final, rb_intern("x"), 0));
  double m02 = value_to_double(rb_funcall(forward_norm, rb_intern("x"), 0));
  double m10 = value_to_double(rb_funcall(right, rb_intern("y"), 0));
  double m11 = value_to_double(rb_funcall(up_final, rb_intern("y"), 0));
  double m12 = value_to_double(rb_funcall(forward_norm, rb_intern("y"), 0));
  double m20 = value_to_double(rb_funcall(right, rb_intern("z"), 0));
  double m21 = value_to_double(rb_funcall(up_final, rb_intern("z"), 0));
  double m22 = value_to_double(rb_funcall(forward_norm, rb_intern("z"), 0));

  double trace = m00 + m11 + m22;
  if (trace > 0.0) {
    double s = 0.5 / sqrt(trace + 1.0);
    return quat_build(klass, (m21 - m12) * s, (m02 - m20) * s,
                      (m10 - m01) * s, 0.25 / s);
  }
  if (m00 > m11 && m00 > m22) {
    double s = 2.0 * sqrt(1.0 + m00 - m11 - m22);
    return quat_build(klass, 0.25 * s, (m01 + m10) / s, (m02 + m20) / s,
                      (m21 - m12) / s);
  }
  if (m11 > m22) {
    double s = 2.0 * sqrt(1.0 + m11 - m00 - m22);
    return quat_build(klass, (m01 + m10) / s, 0.25 * s, (m12 + m21) / s,
                      (m02 - m20) / s);
  }
  double s = 2.0 * sqrt(1.0 + m22 - m00 - m11);
  return quat_build(klass, (m02 + m20) / s, (m12 + m21) / s, 0.25 * s,
                    (m10 - m01) / s);
}

static VALUE quat_get_x(VALUE self) {
  QuatData *data = quat_get(self);
  return DBL2NUM(data->x);
}

static VALUE quat_set_x(VALUE self, VALUE value) {
  QuatData *data = quat_get(self);
  data->x = value_to_double(value);
  return value;
}

static VALUE quat_get_y(VALUE self) {
  QuatData *data = quat_get(self);
  return DBL2NUM(data->y);
}

static VALUE quat_set_y(VALUE self, VALUE value) {
  QuatData *data = quat_get(self);
  data->y = value_to_double(value);
  return value;
}

static VALUE quat_get_z(VALUE self) {
  QuatData *data = quat_get(self);
  return DBL2NUM(data->z);
}

static VALUE quat_set_z(VALUE self, VALUE value) {
  QuatData *data = quat_get(self);
  data->z = value_to_double(value);
  return value;
}

static VALUE quat_get_w(VALUE self) {
  QuatData *data = quat_get(self);
  return DBL2NUM(data->w);
}

static VALUE quat_set_w(VALUE self, VALUE value) {
  QuatData *data = quat_get(self);
  data->w = value_to_double(value);
  return value;
}

VALUE quat_mul(VALUE self, VALUE other) {
  QuatData *a = quat_get(self);

  if (rb_obj_is_kind_of(other, cQuat)) {
    QuatData *b = quat_get(other);
    return quat_build(
        rb_obj_class(self), a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y,
        a->w * b->y - a->x * b->z + a->y * b->w + a->z * b->x,
        a->w * b->z + a->x * b->y - a->y * b->x + a->z * b->w,
        a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z);
  }

  if (rb_obj_is_kind_of(other, cVec3)) {
    double vx = value_to_double(rb_funcall(other, rb_intern("x"), 0));
    double vy = value_to_double(rb_funcall(other, rb_intern("y"), 0));
    double vz = value_to_double(rb_funcall(other, rb_intern("z"), 0));

    double uvx = a->y * vz - a->z * vy;
    double uvy = a->z * vx - a->x * vz;
    double uvz = a->x * vy - a->y * vx;

    double uuvx = a->y * uvz - a->z * uvy;
    double uuvy = a->z * uvx - a->x * uvz;
    double uuvz = a->x * uvy - a->y * uvx;

    double rx = vx + (uvx * a->w + uuvx) * 2.0;
    double ry = vy + (uvy * a->w + uuvy) * 2.0;
    double rz = vz + (uvz * a->w + uuvz) * 2.0;

    return rb_funcall(cVec3, rb_intern("new"), 3, DBL2NUM(rx), DBL2NUM(ry),
                      DBL2NUM(rz));
  }

  if (rb_obj_is_kind_of(other, rb_cNumeric)) {
    double s = value_to_double(other);
    return quat_build(rb_obj_class(self), a->x * s, a->y * s, a->z * s,
                      a->w * s);
  }

  return Qnil;
}

VALUE quat_add(VALUE self, VALUE other) {
  QuatData *a = quat_get(self);
  QuatData *b = quat_get(other);
  return quat_build(rb_obj_class(self), a->x + b->x, a->y + b->y,
                    a->z + b->z, a->w + b->w);
}

VALUE quat_sub(VALUE self, VALUE other) {
  QuatData *a = quat_get(self);
  QuatData *b = quat_get(other);
  return quat_build(rb_obj_class(self), a->x - b->x, a->y - b->y,
                    a->z - b->z, a->w - b->w);
}

VALUE quat_negate(VALUE self) {
  QuatData *a = quat_get(self);
  return quat_build(rb_obj_class(self), -a->x, -a->y, -a->z, -a->w);
}

VALUE quat_dot(VALUE self, VALUE other) {
  QuatData *a = quat_get(self);
  QuatData *b = quat_get(other);
  return DBL2NUM(a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w);
}

VALUE quat_length(VALUE self) {
  QuatData *a = quat_get(self);
  return DBL2NUM(sqrt(a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w));
}

VALUE quat_length_squared(VALUE self) {
  QuatData *a = quat_get(self);
  return DBL2NUM(a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w);
}

VALUE quat_normalize(VALUE self) {
  QuatData *a = quat_get(self);
  double x = a->x;
  double y = a->y;
  double z = a->z;
  double w = a->w;
  normalize_quat(&x, &y, &z, &w);
  return quat_build(rb_obj_class(self), x, y, z, w);
}

VALUE quat_normalize_bang(VALUE self) {
  QuatData *a = quat_get(self);
  normalize_quat(&a->x, &a->y, &a->z, &a->w);
  return self;
}

VALUE quat_conjugate(VALUE self) {
  QuatData *a = quat_get(self);
  return quat_build(rb_obj_class(self), -a->x, -a->y, -a->z, a->w);
}

VALUE quat_inverse(VALUE self) {
  QuatData *a = quat_get(self);
  double len_sq = a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w;
  return quat_build(rb_obj_class(self), -a->x / len_sq, -a->y / len_sq,
                    -a->z / len_sq, a->w / len_sq);
}

VALUE quat_lerp(VALUE self, VALUE other, VALUE t) {
  QuatData *a = quat_get(self);
  QuatData *b = quat_get(other);
  double s = value_to_double(t);
  double x = a->x + (b->x - a->x) * s;
  double y = a->y + (b->y - a->y) * s;
  double z = a->z + (b->z - a->z) * s;
  double w = a->w + (b->w - a->w) * s;
  normalize_quat(&x, &y, &z, &w);
  return quat_build(rb_obj_class(self), x, y, z, w);
}

VALUE quat_slerp(VALUE self, VALUE other, VALUE t) {
  QuatData *a = quat_get(self);
  QuatData *b = quat_get(other);
  double s = value_to_double(t);
  double dot = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;

  double ox = b->x;
  double oy = b->y;
  double oz = b->z;
  double ow = b->w;
  if (dot < 0.0) {
    dot = -dot;
    ox = -ox;
    oy = -oy;
    oz = -oz;
    ow = -ow;
  }

  if (dot > 0.9995) {
    double x = a->x + (ox - a->x) * s;
    double y = a->y + (oy - a->y) * s;
    double z = a->z + (oz - a->z) * s;
    double w = a->w + (ow - a->w) * s;
    normalize_quat(&x, &y, &z, &w);
    return quat_build(rb_obj_class(self), x, y, z, w);
  }

  double theta0 = acos(clamp_double(dot, -1.0, 1.0));
  double theta = theta0 * s;
  double sin_theta = sin(theta);
  double sin_theta0 = sin(theta0);
  double s0 = cos(theta) - dot * sin_theta / sin_theta0;
  double s1 = sin_theta / sin_theta0;

  return quat_build(rb_obj_class(self), a->x * s0 + ox * s1,
                    a->y * s0 + oy * s1, a->z * s0 + oz * s1,
                    a->w * s0 + ow * s1);
}

VALUE quat_to_axis_angle(VALUE self) {
  QuatData *a = quat_get(self);
  double w = clamp_double(a->w, -1.0, 1.0);
  double angle = 2.0 * acos(w);
  double s = sqrt(1.0 - w * w);
  VALUE axis;
  if (s < 0.001) {
    axis = rb_funcall(cVec3, rb_intern("new"), 3, DBL2NUM(1.0), DBL2NUM(0.0),
                      DBL2NUM(0.0));
  } else {
    axis = rb_funcall(cVec3, rb_intern("new"), 3, DBL2NUM(a->x / s),
                      DBL2NUM(a->y / s), DBL2NUM(a->z / s));
  }
  VALUE ary = rb_ary_new_capa(2);
  rb_ary_push(ary, axis);
  rb_ary_push(ary, DBL2NUM(angle));
  return ary;
}

VALUE quat_to_euler(VALUE self) {
  QuatData *a = quat_get(self);
  double sinr_cosp = 2.0 * (a->w * a->x + a->y * a->z);
  double cosr_cosp = 1.0 - 2.0 * (a->x * a->x + a->y * a->y);
  double roll = atan2(sinr_cosp, cosr_cosp);

  double sinp = 2.0 * (a->w * a->y - a->z * a->x);
  double pitch;
  double pi = acos(-1.0);
  if (fabs(sinp) >= 1.0) {
    pitch = copysign(pi / 2.0, sinp);
  } else {
    pitch = asin(sinp);
  }

  double siny_cosp = 2.0 * (a->w * a->z + a->x * a->y);
  double cosy_cosp = 1.0 - 2.0 * (a->y * a->y + a->z * a->z);
  double yaw = atan2(siny_cosp, cosy_cosp);

  return rb_funcall(cVec3, rb_intern("new"), 3, DBL2NUM(roll), DBL2NUM(pitch),
                    DBL2NUM(yaw));
}

VALUE quat_to_mat4(VALUE self) {
  VALUE mat4_class = rb_const_get(mLarb, rb_intern("Mat4"));
  return rb_funcall(mat4_class, rb_intern("from_quaternion"), 1, self);
}

VALUE quat_to_a(VALUE self) {
  QuatData *a = quat_get(self);
  VALUE ary = rb_ary_new_capa(4);
  rb_ary_push(ary, DBL2NUM(a->x));
  rb_ary_push(ary, DBL2NUM(a->y));
  rb_ary_push(ary, DBL2NUM(a->z));
  rb_ary_push(ary, DBL2NUM(a->w));
  return ary;
}

VALUE quat_aref(VALUE self, VALUE index) {
  VALUE ary = quat_to_a(self);
  return rb_ary_entry(ary, NUM2LONG(index));
}

VALUE quat_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cQuat)) {
    return Qfalse;
  }
  QuatData *a = quat_get(self);
  QuatData *b = quat_get(other);
  return (a->x == b->x && a->y == b->y && a->z == b->z && a->w == b->w)
             ? Qtrue
             : Qfalse;
}

VALUE quat_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  QuatData *a = quat_get(self);
  QuatData *b = quat_get(other);
  double eps = NIL_P(epsilon) ? 1e-6 : value_to_double(epsilon);

  if (fabs(a->x - b->x) < eps && fabs(a->y - b->y) < eps &&
      fabs(a->z - b->z) < eps && fabs(a->w - b->w) < eps) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE quat_inspect(VALUE self) {
  QuatData *a = quat_get(self);
  VALUE sx = rb_funcall(DBL2NUM(a->x), rb_intern("to_s"), 0);
  VALUE sy = rb_funcall(DBL2NUM(a->y), rb_intern("to_s"), 0);
  VALUE sz = rb_funcall(DBL2NUM(a->z), rb_intern("to_s"), 0);
  VALUE sw = rb_funcall(DBL2NUM(a->w), rb_intern("to_s"), 0);
  VALUE str = rb_str_new_cstr("Quat[");
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

void Init_quat(VALUE module) {
  cQuat = rb_define_class_under(module, "Quat", rb_cObject);
  cVec3 = rb_const_get(mLarb, rb_intern("Vec3"));

  rb_define_alloc_func(cQuat, quat_alloc);
  rb_define_method(cQuat, "initialize", quat_initialize, -1);

  rb_define_singleton_method(cQuat, "[]", quat_class_bracket, 4);
  rb_define_singleton_method(cQuat, "identity", quat_class_identity, 0);
  rb_define_singleton_method(cQuat, "from_axis_angle",
                             quat_class_from_axis_angle, 2);
  rb_define_singleton_method(cQuat, "from_euler", quat_class_from_euler, 3);
  rb_define_singleton_method(cQuat, "look_rotation", quat_class_look_rotation,
                             -1);

  rb_define_method(cQuat, "x", quat_get_x, 0);
  rb_define_method(cQuat, "x=", quat_set_x, 1);
  rb_define_method(cQuat, "y", quat_get_y, 0);
  rb_define_method(cQuat, "y=", quat_set_y, 1);
  rb_define_method(cQuat, "z", quat_get_z, 0);
  rb_define_method(cQuat, "z=", quat_set_z, 1);
  rb_define_method(cQuat, "w", quat_get_w, 0);
  rb_define_method(cQuat, "w=", quat_set_w, 1);

  rb_define_method(cQuat, "*", quat_mul, 1);
  rb_define_method(cQuat, "+", quat_add, 1);
  rb_define_method(cQuat, "-", quat_sub, 1);
  rb_define_method(cQuat, "-@", quat_negate, 0);
  rb_define_method(cQuat, "dot", quat_dot, 1);
  rb_define_method(cQuat, "length", quat_length, 0);
  rb_define_method(cQuat, "length_squared", quat_length_squared, 0);
  rb_define_method(cQuat, "normalize", quat_normalize, 0);
  rb_define_method(cQuat, "normalize!", quat_normalize_bang, 0);
  rb_define_method(cQuat, "conjugate", quat_conjugate, 0);
  rb_define_method(cQuat, "inverse", quat_inverse, 0);
  rb_define_method(cQuat, "lerp", quat_lerp, 2);
  rb_define_method(cQuat, "slerp", quat_slerp, 2);
  rb_define_method(cQuat, "to_axis_angle", quat_to_axis_angle, 0);
  rb_define_method(cQuat, "to_euler", quat_to_euler, 0);
  rb_define_method(cQuat, "to_mat4", quat_to_mat4, 0);
  rb_define_method(cQuat, "to_a", quat_to_a, 0);
  rb_define_method(cQuat, "[]", quat_aref, 1);
  rb_define_method(cQuat, "==", quat_equal, 1);
  rb_define_method(cQuat, "near?", quat_near, -1);
  rb_define_method(cQuat, "inspect", quat_inspect, 0);
  rb_define_alias(cQuat, "to_s", "inspect");
}
