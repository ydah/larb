# frozen_string_literal: true

require_relative "../test_helper"

class Vec3Test < Test::Unit::TestCase
  def test_new_with_default_values
    v = Larb::Vec3.new
    assert_equal 0.0, v.x
    assert_equal 0.0, v.y
    assert_equal 0.0, v.z
  end

  def test_new_with_given_values
    v = Larb::Vec3.new(1, 2, 3)
    assert_equal 1.0, v.x
    assert_equal 2.0, v.y
    assert_equal 3.0, v.z
  end

  def test_bracket_syntax
    v = Larb::Vec3[1, 2, 3]
    assert_equal 1.0, v.x
    assert_equal 2.0, v.y
    assert_equal 3.0, v.z
  end

  def test_zero
    assert_equal Larb::Vec3.new(0, 0, 0), Larb::Vec3.zero
  end

  def test_one
    assert_equal Larb::Vec3.new(1, 1, 1), Larb::Vec3.one
  end

  def test_up
    assert_equal Larb::Vec3.new(0, 1, 0), Larb::Vec3.up
  end

  def test_down
    assert_equal Larb::Vec3.new(0, -1, 0), Larb::Vec3.down
  end

  def test_forward
    assert_equal Larb::Vec3.new(0, 0, -1), Larb::Vec3.forward
  end

  def test_back
    assert_equal Larb::Vec3.new(0, 0, 1), Larb::Vec3.back
  end

  def test_right
    assert_equal Larb::Vec3.new(1, 0, 0), Larb::Vec3.right
  end

  def test_left
    assert_equal Larb::Vec3.new(-1, 0, 0), Larb::Vec3.left
  end

  def test_add
    v1 = Larb::Vec3.new(1, 2, 3)
    v2 = Larb::Vec3.new(4, 5, 6)
    assert_equal Larb::Vec3.new(5, 7, 9), v1 + v2
  end

  def test_subtract
    v1 = Larb::Vec3.new(4, 5, 6)
    v2 = Larb::Vec3.new(1, 2, 3)
    assert_equal Larb::Vec3.new(3, 3, 3), v1 - v2
  end

  def test_multiply_by_scalar
    v = Larb::Vec3.new(1, 2, 3)
    assert_equal Larb::Vec3.new(2, 4, 6), v * 2
  end

  def test_multiply_by_vec3
    v1 = Larb::Vec3.new(1, 2, 3)
    v2 = Larb::Vec3.new(2, 3, 4)
    assert_equal Larb::Vec3.new(2, 6, 12), v1 * v2
  end

  def test_divide_by_scalar
    v = Larb::Vec3.new(2, 4, 6)
    assert_equal Larb::Vec3.new(1, 2, 3), v / 2
  end

  def test_negate
    v = Larb::Vec3.new(1, -2, 3)
    assert_equal Larb::Vec3.new(-1, 2, -3), -v
  end

  def test_dot
    v1 = Larb::Vec3.new(1, 2, 3)
    v2 = Larb::Vec3.new(4, 5, 6)
    assert_equal 32.0, v1.dot(v2)
  end

  def test_cross
    v1 = Larb::Vec3.new(1, 0, 0)
    v2 = Larb::Vec3.new(0, 1, 0)
    assert_equal Larb::Vec3.new(0, 0, 1), v1.cross(v2)
  end

  def test_cross_arbitrary
    v1 = Larb::Vec3.new(1, 2, 3)
    v2 = Larb::Vec3.new(4, 5, 6)
    assert_equal Larb::Vec3.new(-3, 6, -3), v1.cross(v2)
  end

  def test_length
    v = Larb::Vec3.new(2, 3, 6)
    assert_equal 7.0, v.length
  end

  def test_length_squared
    v = Larb::Vec3.new(2, 3, 6)
    assert_equal 49.0, v.length_squared
  end

  def test_normalize
    v = Larb::Vec3.new(0, 3, 4)
    result = v.normalize
    assert_in_delta 0.0, result.x, 1e-10
    assert_in_delta 0.6, result.y, 1e-10
    assert_in_delta 0.8, result.z, 1e-10
  end

  def test_reflect
    v = Larb::Vec3.new(1, -1, 0).normalize
    normal = Larb::Vec3.new(0, 1, 0)
    result = v.reflect(normal)
    assert_in_delta v.x, result.x, 1e-10
    assert_in_delta(-v.y, result.y, 1e-10)
  end

  def test_xy
    v = Larb::Vec3.new(1, 2, 3)
    result = v.xy
    assert_instance_of Larb::Vec2, result
    assert_equal 1.0, result.x
    assert_equal 2.0, result.y
  end

  def test_xz
    v = Larb::Vec3.new(1, 2, 3)
    result = v.xz
    assert_instance_of Larb::Vec2, result
    assert_equal 1.0, result.x
    assert_equal 3.0, result.y
  end

  def test_yz
    v = Larb::Vec3.new(1, 2, 3)
    result = v.yz
    assert_instance_of Larb::Vec2, result
    assert_equal 2.0, result.x
    assert_equal 3.0, result.y
  end

  def test_lerp
    v1 = Larb::Vec3.new(0, 0, 0)
    v2 = Larb::Vec3.new(10, 20, 30)
    assert_equal Larb::Vec3.new(5, 10, 15), v1.lerp(v2, 0.5)
  end

  def test_to_a
    v = Larb::Vec3.new(1, 2, 3)
    assert_equal [1.0, 2.0, 3.0], v.to_a
  end

  def test_to_vec4
    v = Larb::Vec3.new(1, 2, 3)
    result = v.to_vec4
    assert_instance_of Larb::Vec4, result
    assert_equal 1.0, result.x
    assert_equal 2.0, result.y
    assert_equal 3.0, result.z
    assert_equal 1.0, result.w
  end

  def test_to_vec4_with_custom_w
    v = Larb::Vec3.new(1, 2, 3)
    result = v.to_vec4(0.5)
    assert_equal 0.5, result.w
  end

  def test_index_access
    v = Larb::Vec3.new(1, 2, 3)
    assert_equal 1.0, v[0]
    assert_equal 2.0, v[1]
    assert_equal 3.0, v[2]
  end

  def test_equality
    v1 = Larb::Vec3.new(1, 2, 3)
    v2 = Larb::Vec3.new(1, 2, 3)
    assert_equal v1, v2
  end

  def test_inequality
    v1 = Larb::Vec3.new(1, 2, 3)
    v2 = Larb::Vec3.new(1, 2, 4)
    assert_not_equal v1, v2
  end

  def test_not_equal_to_array
    v = Larb::Vec3.new(1, 2, 3)
    assert_not_equal v, [1, 2, 3]
  end

  def test_near
    v1 = Larb::Vec3.new(1.0, 2.0, 3.0)
    v2 = Larb::Vec3.new(1.0000001, 2.0000001, 3.0000001)
    assert v1.near?(v2)
  end

  def test_distance
    v1 = Larb::Vec3.new(0, 0, 0)
    v2 = Larb::Vec3.new(2, 3, 6)
    assert_equal 7.0, v1.distance(v2)
  end

  def test_distance_squared
    v1 = Larb::Vec3.new(0, 0, 0)
    v2 = Larb::Vec3.new(2, 3, 6)
    assert_equal 49.0, v1.distance_squared(v2)
  end

  def test_angle_between
    v1 = Larb::Vec3.new(1, 0, 0)
    v2 = Larb::Vec3.new(0, 1, 0)
    assert_in_delta Math::PI / 2, v1.angle_between(v2), 1e-10
  end

  def test_project
    v = Larb::Vec3.new(3, 4, 0)
    onto = Larb::Vec3.new(1, 0, 0)
    result = v.project(onto)
    assert_equal 3.0, result.x
    assert_equal 0.0, result.y
  end

  def test_reject
    v = Larb::Vec3.new(3, 4, 0)
    from = Larb::Vec3.new(1, 0, 0)
    result = v.reject(from)
    assert_equal 0.0, result.x
    assert_equal 4.0, result.y
  end

  def test_clamp_length
    v = Larb::Vec3.new(2, 3, 6)
    result = v.clamp_length(3.5)
    assert_in_delta 3.5, result.length, 1e-10
  end

  def test_clamp_length_under_limit
    v = Larb::Vec3.new(1, 1, 1)
    result = v.clamp_length(10)
    assert_equal v, result
  end

  def test_normalize!
    v = Larb::Vec3.new(0, 3, 4)
    v.normalize!
    assert_in_delta 1.0, v.length, 1e-10
  end

  def test_min
    v1 = Larb::Vec3.new(1, 5, 3)
    v2 = Larb::Vec3.new(2, 3, 4)
    assert_equal Larb::Vec3.new(1, 3, 3), v1.min(v2)
  end

  def test_max
    v1 = Larb::Vec3.new(1, 5, 3)
    v2 = Larb::Vec3.new(2, 3, 4)
    assert_equal Larb::Vec3.new(2, 5, 4), v1.max(v2)
  end

  def test_abs
    v = Larb::Vec3.new(-1, -2, 3)
    assert_equal Larb::Vec3.new(1, 2, 3), v.abs
  end

  def test_floor
    v = Larb::Vec3.new(1.7, 2.3, 3.9)
    assert_equal Larb::Vec3.new(1, 2, 3), v.floor
  end

  def test_ceil
    v = Larb::Vec3.new(1.1, 2.5, 3.9)
    assert_equal Larb::Vec3.new(2, 3, 4), v.ceil
  end

  def test_round
    v = Larb::Vec3.new(1.4, 2.5, 3.6)
    assert_equal Larb::Vec3.new(1, 3, 4), v.round
  end

  def test_inspect
    v = Larb::Vec3.new(1, 2, 3)
    assert_equal "Vec3[1.0, 2.0, 3.0]", v.inspect
  end

  def test_to_s
    v = Larb::Vec3.new(1, 2, 3)
    assert_equal v.inspect, v.to_s
  end
end
