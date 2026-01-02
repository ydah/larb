# frozen_string_literal: true

require_relative "../test_helper"

class Mat4Test < Test::Unit::TestCase
  def test_new_identity_by_default
    m = Larb::Mat4.new
    assert_equal 1.0, m[0]
    assert_equal 1.0, m[5]
    assert_equal 1.0, m[10]
    assert_equal 1.0, m[15]
    assert_equal 0.0, m[1]
    assert_equal 0.0, m[4]
  end

  def test_new_with_data
    data = Array.new(16) { |i| i.to_f }
    m = Larb::Mat4.new(data)
    assert_equal data, m.data
  end

  def test_identity
    m = Larb::Mat4.identity
    assert_equal 1.0, m[0]
    assert_equal 1.0, m[5]
    assert_equal 1.0, m[10]
    assert_equal 1.0, m[15]
  end

  def test_zero
    m = Larb::Mat4.zero
    16.times { |i| assert_equal 0.0, m[i] }
  end

  def test_translation
    m = Larb::Mat4.translation(1, 2, 3)
    assert_equal 1.0, m[12]
    assert_equal 2.0, m[13]
    assert_equal 3.0, m[14]
    assert_equal 1.0, m[0]
    assert_equal 1.0, m[5]
    assert_equal 1.0, m[10]
    assert_equal 1.0, m[15]
  end

  def test_scaling
    m = Larb::Mat4.scaling(2, 3, 4)
    assert_equal 2.0, m[0]
    assert_equal 3.0, m[5]
    assert_equal 4.0, m[10]
    assert_equal 1.0, m[15]
  end

  def test_rotation_x
    m = Larb::Mat4.rotation_x(Math::PI / 2)
    assert_equal 1.0, m[0]
    assert_in_delta 0.0, m[5], 1e-10
    assert_in_delta 1.0, m[6], 1e-10
    assert_in_delta(-1.0, m[9], 1e-10)
    assert_in_delta 0.0, m[10], 1e-10
  end

  def test_rotation_y
    m = Larb::Mat4.rotation_y(Math::PI / 2)
    assert_in_delta 0.0, m[0], 1e-10
    assert_in_delta(-1.0, m[2], 1e-10)
    assert_equal 1.0, m[5]
    assert_in_delta 1.0, m[8], 1e-10
    assert_in_delta 0.0, m[10], 1e-10
  end

  def test_rotation_z
    m = Larb::Mat4.rotation_z(Math::PI / 2)
    assert_in_delta 0.0, m[0], 1e-10
    assert_in_delta 1.0, m[1], 1e-10
    assert_in_delta(-1.0, m[4], 1e-10)
    assert_in_delta 0.0, m[5], 1e-10
    assert_equal 1.0, m[10]
  end

  def test_rotation
    m = Larb::Mat4.rotation(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    assert_in_delta 0.0, m[0], 1e-10
    assert_in_delta 1.0, m[8], 1e-10
  end

  def test_look_at
    eye = Larb::Vec3.new(0, 0, 5)
    target = Larb::Vec3.new(0, 0, 0)
    up = Larb::Vec3.new(0, 1, 0)
    m = Larb::Mat4.look_at(eye, target, up)
    assert_instance_of Larb::Mat4, m
    assert_in_delta(-5.0, m[14], 1e-10)
  end

  def test_perspective
    m = Larb::Mat4.perspective(Math::PI / 4, 16.0 / 9.0, 0.1, 100.0)
    assert_instance_of Larb::Mat4, m
    assert_equal(-1.0, m[11])
  end

  def test_orthographic
    m = Larb::Mat4.orthographic(-1, 1, -1, 1, 0.1, 100)
    assert_instance_of Larb::Mat4, m
    assert_equal 1.0, m[0]
    assert_equal 1.0, m[5]
  end

  def test_index_access
    m = Larb::Mat4.identity
    assert_equal 1.0, m[0]
    assert_equal 0.0, m[1]
  end

  def test_index_assignment
    m = Larb::Mat4.identity
    m[0] = 5.0
    assert_equal 5.0, m[0]
  end

  def test_multiply_mat4
    m1 = Larb::Mat4.translation(1, 0, 0)
    m2 = Larb::Mat4.translation(0, 1, 0)
    result = m1 * m2
    assert_equal 1.0, result[12]
    assert_equal 1.0, result[13]
  end

  def test_multiply_identity
    m = Larb::Mat4.translation(1, 2, 3)
    result = Larb::Mat4.identity * m
    assert_equal m.data, result.data
  end

  def test_multiply_vec4
    m = Larb::Mat4.translation(1, 2, 3)
    v = Larb::Vec4.new(0, 0, 0, 1)
    result = m * v
    assert_equal 1.0, result.x
    assert_equal 2.0, result.y
    assert_equal 3.0, result.z
    assert_equal 1.0, result.w
  end

  def test_multiply_vec3
    m = Larb::Mat4.translation(1, 2, 3)
    v = Larb::Vec3.new(0, 0, 0)
    result = m * v
    assert_equal 1.0, result.x
    assert_equal 2.0, result.y
    assert_equal 3.0, result.z
  end

  def test_transpose
    data = Array.new(16) { |i| i.to_f }
    m = Larb::Mat4.new(data)
    result = m.transpose
    assert_equal 4.0, result[1]
    assert_equal 1.0, result[4]
    assert_equal 8.0, result[2]
    assert_equal 2.0, result[8]
  end

  def test_inverse
    m = Larb::Mat4.translation(1, 2, 3)
    inv = m.inverse
    result = m * inv
    assert_in_delta 1.0, result[0], 1e-10
    assert_in_delta 1.0, result[5], 1e-10
    assert_in_delta 1.0, result[10], 1e-10
    assert_in_delta 1.0, result[15], 1e-10
    assert_in_delta 0.0, result[12], 1e-10
    assert_in_delta 0.0, result[13], 1e-10
    assert_in_delta 0.0, result[14], 1e-10
  end

  def test_inverse_singular
    m = Larb::Mat4.zero
    assert_raise_message("Matrix is not invertible") { m.inverse }
  end

  def test_to_a
    m = Larb::Mat4.identity
    arr = m.to_a
    assert_equal m.data, arr
    arr[0] = 999
    assert_equal 1.0, m.data[0]
  end

  def test_determinant
    m = Larb::Mat4.identity
    assert_equal 1.0, m.determinant
  end

  def test_determinant_zero
    m = Larb::Mat4.zero
    assert_equal 0.0, m.determinant
  end

  def test_determinant_scaling
    m = Larb::Mat4.scaling(2, 3, 4)
    assert_equal 24.0, m.determinant
  end

  def test_add
    m1 = Larb::Mat4.identity
    m2 = Larb::Mat4.identity
    result = m1 + m2
    assert_equal 2.0, result[0]
    assert_equal 2.0, result[5]
  end

  def test_subtract
    m1 = Larb::Mat4.identity
    m2 = Larb::Mat4.identity
    result = m1 - m2
    assert_equal 0.0, result[0]
    assert_equal 0.0, result[5]
  end

  def test_equality
    m1 = Larb::Mat4.identity
    m2 = Larb::Mat4.identity
    assert_equal m1, m2
  end

  def test_inequality
    m1 = Larb::Mat4.identity
    m2 = Larb::Mat4.translation(1, 0, 0)
    assert_not_equal m1, m2
  end

  def test_near
    m1 = Larb::Mat4.identity
    m2 = Larb::Mat4.identity
    m2[0] = 1.0000001
    assert m1.near?(m2)
  end

  def test_extract_translation
    m = Larb::Mat4.translation(1, 2, 3)
    result = m.extract_translation
    assert_equal Larb::Vec3.new(1, 2, 3), result
  end

  def test_extract_scale
    m = Larb::Mat4.scaling(2, 3, 4)
    result = m.extract_scale
    assert_in_delta 2.0, result.x, 1e-10
    assert_in_delta 3.0, result.y, 1e-10
    assert_in_delta 4.0, result.z, 1e-10
  end

  def test_frustum
    m = Larb::Mat4.frustum(-1, 1, -1, 1, 0.1, 100)
    assert_instance_of Larb::Mat4, m
  end

  def test_from_quaternion
    q = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    m = Larb::Mat4.from_quaternion(q)
    assert_instance_of Larb::Mat4, m
    assert_in_delta 0.0, m[0], 1e-10
  end

  def test_inspect
    m = Larb::Mat4.identity
    assert_match(/Mat4/, m.inspect)
  end
end
