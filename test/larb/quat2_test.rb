# frozen_string_literal: true

require_relative "../test_helper"

class Quat2Test < Test::Unit::TestCase
  def test_new_identity_by_default
    q = Larb::Quat2.new
    assert_equal 1.0, q[3]
    assert_equal 0.0, q[7]
  end

  def test_new_with_data
    data = [0, 0, 0, 1, 0, 0, 0, 0]
    q = Larb::Quat2.new(data)
    assert_equal data, q.data
  end

  def test_identity
    q = Larb::Quat2.identity
    assert_equal Larb::Quat.identity, q.real
  end

  def test_from_rotation_translation
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    translation = Larb::Vec3.new(1, 2, 3)
    q = Larb::Quat2.from_rotation_translation(rotation, translation)
    assert q.rotation.near?(rotation)
    assert q.translation.near?(translation)
  end

  def test_from_translation
    translation = Larb::Vec3.new(5, 10, 15)
    q = Larb::Quat2.from_translation(translation)
    assert q.translation.near?(translation)
    assert q.rotation.near?(Larb::Quat.identity)
  end

  def test_from_rotation
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    q = Larb::Quat2.from_rotation(rotation)
    assert q.rotation.near?(rotation)
    assert q.translation.near?(Larb::Vec3.zero)
  end

  def test_real
    q = Larb::Quat2.identity
    assert_instance_of Larb::Quat, q.real
  end

  def test_dual
    q = Larb::Quat2.identity
    assert_instance_of Larb::Quat, q.dual
  end

  def test_multiply_quat2
    q1 = Larb::Quat2.from_translation(Larb::Vec3.new(1, 0, 0))
    q2 = Larb::Quat2.from_translation(Larb::Vec3.new(0, 1, 0))
    result = q1 * q2
    assert result.translation.near?(Larb::Vec3.new(1, 1, 0))
  end

  def test_multiply_vec3
    q = Larb::Quat2.from_translation(Larb::Vec3.new(10, 0, 0))
    point = Larb::Vec3.new(0, 0, 0)
    result = q * point
    assert result.near?(Larb::Vec3.new(10, 0, 0))
  end

  def test_add
    q1 = Larb::Quat2.identity
    q2 = Larb::Quat2.identity
    result = q1 + q2
    assert_equal 2.0, result[3]
  end

  def test_subtract
    q1 = Larb::Quat2.identity
    q2 = Larb::Quat2.identity
    result = q1 - q2
    assert_equal 0.0, result[3]
  end

  def test_dot
    q = Larb::Quat2.identity
    assert_equal 1.0, q.dot(q)
  end

  def test_length
    q = Larb::Quat2.identity
    assert_equal 1.0, q.length
  end

  def test_normalize
    q = Larb::Quat2.new([0, 0, 0, 2, 0, 0, 0, 0])
    result = q.normalize
    assert_in_delta 1.0, result.length, 1e-10
  end

  def test_conjugate
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    q = Larb::Quat2.from_rotation(rotation)
    conj = q.conjugate
    result = q * conj
    assert result.rotation.near?(Larb::Quat.identity)
  end

  def test_inverse
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 4)
    translation = Larb::Vec3.new(1, 2, 3)
    q = Larb::Quat2.from_rotation_translation(rotation, translation)
    inv = q.inverse
    result = q * inv
    assert result.rotation.near?(Larb::Quat.identity)
  end

  def test_translation
    t = Larb::Vec3.new(5, 10, 15)
    q = Larb::Quat2.from_translation(t)
    assert q.translation.near?(t)
  end

  def test_rotation
    r = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    q = Larb::Quat2.from_rotation(r)
    assert q.rotation.near?(r)
  end

  def test_transform_point
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    translation = Larb::Vec3.new(0, 0, 5)
    q = Larb::Quat2.from_rotation_translation(rotation, translation)
    point = Larb::Vec3.new(1, 0, 0)
    result = q.transform_point(point)
    assert_in_delta 0.0, result.x, 1e-10
    assert_in_delta 4.0, result.z, 1e-10
  end

  def test_lerp
    q1 = Larb::Quat2.from_translation(Larb::Vec3.new(0, 0, 0))
    q2 = Larb::Quat2.from_translation(Larb::Vec3.new(10, 0, 0))
    result = q1.lerp(q2, 0.5)
    assert_in_delta 5.0, result.translation.x, 1e-6
  end

  def test_equality
    q1 = Larb::Quat2.identity
    q2 = Larb::Quat2.identity
    assert_equal q1, q2
  end

  def test_near
    q1 = Larb::Quat2.identity
    q2 = Larb::Quat2.new([0, 0, 0, 1.0000001, 0, 0, 0, 0])
    assert q1.near?(q2)
  end

  def test_inspect
    q = Larb::Quat2.identity
    assert_match(/Quat2/, q.inspect)
  end
end
