# frozen_string_literal: true

require_relative "../test_helper"

class Mat2Test < Test::Unit::TestCase
  def test_new_identity_by_default
    m = Larb::Mat2.new
    assert_equal 1.0, m[0]
    assert_equal 0.0, m[1]
    assert_equal 0.0, m[2]
    assert_equal 1.0, m[3]
  end

  def test_new_with_data
    m = Larb::Mat2.new([1, 2, 3, 4])
    assert_equal [1, 2, 3, 4], m.data
  end

  def test_identity
    m = Larb::Mat2.identity
    assert_equal 1.0, m[0]
    assert_equal 1.0, m[3]
  end

  def test_zero
    m = Larb::Mat2.zero
    4.times { |i| assert_equal 0.0, m[i] }
  end

  def test_rotation
    m = Larb::Mat2.rotation(Math::PI / 2)
    assert_in_delta 0.0, m[0], 1e-10
    assert_in_delta 1.0, m[1], 1e-10
    assert_in_delta(-1.0, m[2], 1e-10)
    assert_in_delta 0.0, m[3], 1e-10
  end

  def test_scaling
    m = Larb::Mat2.scaling(2, 3)
    assert_equal 2.0, m[0]
    assert_equal 3.0, m[3]
  end

  def test_from_vec2
    v1 = Larb::Vec2.new(1, 2)
    v2 = Larb::Vec2.new(3, 4)
    m = Larb::Mat2.from_vec2(v1, v2)
    assert_equal [1.0, 2.0, 3.0, 4.0], m.data
  end

  def test_multiply_mat2
    m1 = Larb::Mat2.rotation(Math::PI / 4)
    m2 = Larb::Mat2.rotation(Math::PI / 4)
    result = m1 * m2
    expected = Larb::Mat2.rotation(Math::PI / 2)
    assert result.near?(expected)
  end

  def test_multiply_vec2
    m = Larb::Mat2.rotation(Math::PI / 2)
    v = Larb::Vec2.new(1, 0)
    result = m * v
    assert_in_delta 0.0, result.x, 1e-10
    assert_in_delta 1.0, result.y, 1e-10
  end

  def test_multiply_scalar
    m = Larb::Mat2.identity
    result = m * 2
    assert_equal 2.0, result[0]
    assert_equal 2.0, result[3]
  end

  def test_add
    m1 = Larb::Mat2.identity
    m2 = Larb::Mat2.identity
    result = m1 + m2
    assert_equal 2.0, result[0]
  end

  def test_subtract
    m1 = Larb::Mat2.identity
    m2 = Larb::Mat2.identity
    result = m1 - m2
    assert_equal 0.0, result[0]
  end

  def test_determinant
    m = Larb::Mat2.identity
    assert_equal 1.0, m.determinant
  end

  def test_determinant_scaling
    m = Larb::Mat2.scaling(2, 3)
    assert_equal 6.0, m.determinant
  end

  def test_inverse
    m = Larb::Mat2.scaling(2, 4)
    inv = m.inverse
    result = m * inv
    assert result.near?(Larb::Mat2.identity)
  end

  def test_inverse_singular
    m = Larb::Mat2.zero
    assert_raise_message("Matrix is not invertible") { m.inverse }
  end

  def test_transpose
    m = Larb::Mat2.new([1, 2, 3, 4])
    result = m.transpose
    assert_equal [1, 3, 2, 4], result.data
  end

  def test_adjoint
    m = Larb::Mat2.new([1, 2, 3, 4])
    adj = m.adjoint
    assert_equal 4.0, adj[0]
    assert_equal(-2.0, adj[1])
  end

  def test_frobenius_norm
    m = Larb::Mat2.new([1, 2, 2, 0])
    assert_equal 3.0, m.frobenius_norm
  end

  def test_equality
    m1 = Larb::Mat2.identity
    m2 = Larb::Mat2.identity
    assert_equal m1, m2
  end

  def test_near
    m1 = Larb::Mat2.identity
    m2 = Larb::Mat2.new([1.0000001, 0, 0, 1.0000001])
    assert m1.near?(m2)
  end

  def test_inspect
    m = Larb::Mat2.identity
    assert_match(/Mat2/, m.inspect)
  end
end
