# frozen_string_literal: true

require_relative "../test_helper"

class Mat3Test < Test::Unit::TestCase
  def test_new_identity_by_default
    m = Larb::Mat3.new
    assert_equal 1.0, m[0]
    assert_equal 1.0, m[4]
    assert_equal 1.0, m[8]
  end

  def test_new_with_data
    data = Array.new(9) { |i| i.to_f }
    m = Larb::Mat3.new(data)
    assert_equal data, m.data
  end

  def test_identity
    m = Larb::Mat3.identity
    assert_equal 1.0, m[0]
    assert_equal 1.0, m[4]
    assert_equal 1.0, m[8]
  end

  def test_zero
    m = Larb::Mat3.zero
    9.times { |i| assert_equal 0.0, m[i] }
  end

  def test_from_mat4
    m4 = Larb::Mat4.identity
    m3 = Larb::Mat3.from_mat4(m4)
    assert_equal 1.0, m3[0]
    assert_equal 1.0, m3[4]
    assert_equal 1.0, m3[8]
  end

  def test_from_quaternion
    q = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 0, 1), Math::PI / 2)
    m = Larb::Mat3.from_quaternion(q)
    assert_in_delta 0.0, m[0], 1e-10
    assert_in_delta 1.0, m[1], 1e-10
  end

  def test_translation
    m = Larb::Mat3.translation(5, 10)
    assert_equal 5.0, m[6]
    assert_equal 10.0, m[7]
  end

  def test_rotation
    m = Larb::Mat3.rotation(Math::PI / 2)
    assert_in_delta 0.0, m[0], 1e-10
    assert_in_delta 1.0, m[1], 1e-10
  end

  def test_scaling
    m = Larb::Mat3.scaling(2, 3)
    assert_equal 2.0, m[0]
    assert_equal 3.0, m[4]
  end

  def test_multiply_mat3
    m1 = Larb::Mat3.identity
    m2 = Larb::Mat3.translation(1, 2)
    result = m1 * m2
    assert_equal 1.0, result[6]
    assert_equal 2.0, result[7]
  end

  def test_multiply_vec3
    m = Larb::Mat3.scaling(2, 3)
    v = Larb::Vec3.new(1, 1, 1)
    result = m * v
    assert_equal 2.0, result.x
    assert_equal 3.0, result.y
  end

  def test_determinant
    m = Larb::Mat3.identity
    assert_equal 1.0, m.determinant
  end

  def test_determinant_scaling
    m = Larb::Mat3.scaling(2, 3)
    assert_equal 6.0, m.determinant
  end

  def test_inverse
    m = Larb::Mat3.scaling(2, 4)
    inv = m.inverse
    result = m * inv
    assert result.near?(Larb::Mat3.identity)
  end

  def test_inverse_singular
    m = Larb::Mat3.zero
    assert_raise_message("Matrix is not invertible") { m.inverse }
  end

  def test_transpose
    data = Array.new(9) { |i| i.to_f }
    m = Larb::Mat3.new(data)
    result = m.transpose
    assert_equal 3.0, result[1]
    assert_equal 1.0, result[3]
  end

  def test_translate
    m = Larb::Mat3.identity
    result = m.translate(5, 10)
    assert_equal 5.0, result[6]
    assert_equal 10.0, result[7]
  end

  def test_rotate
    m = Larb::Mat3.identity
    result = m.rotate(Math::PI / 2)
    assert_in_delta 0.0, result[0], 1e-10
  end

  def test_scale
    m = Larb::Mat3.identity
    result = m.scale(2, 3)
    assert_equal 2.0, result[0]
    assert_equal 3.0, result[4]
  end

  def test_equality
    m1 = Larb::Mat3.identity
    m2 = Larb::Mat3.identity
    assert_equal m1, m2
  end

  def test_near
    m1 = Larb::Mat3.identity
    m2 = Larb::Mat3.identity
    m2[0] = 1.0000001
    assert m1.near?(m2)
  end

  def test_inspect
    m = Larb::Mat3.identity
    assert_match(/Mat3/, m.inspect)
  end
end
