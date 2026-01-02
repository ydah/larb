# frozen_string_literal: true

module Larb
  class Mat3
    attr_reader :data

    def initialize(data = nil)
      @data = data || [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
    end

    def self.identity
      new
    end

    def self.zero
      new(Array.new(9, 0.0))
    end

    def self.from_mat4(m)
      new([
        m[0], m[1], m[2],
        m[4], m[5], m[6],
        m[8], m[9], m[10]
      ])
    end

    def self.from_mat2d(m)
      new([
        m[0], m[1], 0,
        m[2], m[3], 0,
        m[4], m[5], 1
      ])
    end

    def self.from_quaternion(q)
      x, y, z, w = q.x, q.y, q.z, q.w
      x2 = x + x
      y2 = y + y
      z2 = z + z
      xx = x * x2
      yx = y * x2
      yy = y * y2
      zx = z * x2
      zy = z * y2
      zz = z * z2
      wx = w * x2
      wy = w * y2
      wz = w * z2

      new([
        1 - yy - zz, yx + wz, zx - wy,
        yx - wz, 1 - xx - zz, zy + wx,
        zx + wy, zy - wx, 1 - xx - yy
      ])
    end

    def self.normal_from_mat4(m)
      from_mat4(m).inverse.transpose
    end

    def self.projection(width, height)
      new([
        2.0 / width, 0, 0,
        0, -2.0 / height, 0,
        -1, 1, 1
      ])
    end

    def self.translation(x, y)
      new([
        1, 0, 0,
        0, 1, 0,
        x.to_f, y.to_f, 1
      ])
    end

    def self.rotation(radians)
      c = Math.cos(radians)
      s = Math.sin(radians)
      new([
        c, s, 0,
        -s, c, 0,
        0, 0, 1
      ])
    end

    def self.scaling(x, y)
      new([
        x.to_f, 0, 0,
        0, y.to_f, 0,
        0, 0, 1
      ])
    end

    def [](i)
      @data[i]
    end

    def []=(i, v)
      @data[i] = v.to_f
    end

    def *(other)
      case other
      when Mat3
        a = @data
        b = other.data
        Mat3.new([
          a[0] * b[0] + a[3] * b[1] + a[6] * b[2],
          a[1] * b[0] + a[4] * b[1] + a[7] * b[2],
          a[2] * b[0] + a[5] * b[1] + a[8] * b[2],
          a[0] * b[3] + a[3] * b[4] + a[6] * b[5],
          a[1] * b[3] + a[4] * b[4] + a[7] * b[5],
          a[2] * b[3] + a[5] * b[4] + a[8] * b[5],
          a[0] * b[6] + a[3] * b[7] + a[6] * b[8],
          a[1] * b[6] + a[4] * b[7] + a[7] * b[8],
          a[2] * b[6] + a[5] * b[7] + a[8] * b[8]
        ])
      when Vec3
        Vec3.new(
          @data[0] * other.x + @data[3] * other.y + @data[6] * other.z,
          @data[1] * other.x + @data[4] * other.y + @data[7] * other.z,
          @data[2] * other.x + @data[5] * other.y + @data[8] * other.z
        )
      when Numeric
        Mat3.new(@data.map { |v| v * other })
      end
    end

    def +(other)
      Mat3.new(@data.zip(other.data).map { |a, b| a + b })
    end

    def -(other)
      Mat3.new(@data.zip(other.data).map { |a, b| a - b })
    end

    def determinant
      a = @data
      a[0] * (a[4] * a[8] - a[5] * a[7]) -
        a[3] * (a[1] * a[8] - a[2] * a[7]) +
        a[6] * (a[1] * a[5] - a[2] * a[4])
    end

    def inverse
      a = @data
      det = determinant
      raise "Matrix is not invertible" if det.abs < 1e-10

      inv_det = 1.0 / det
      Mat3.new([
        (a[4] * a[8] - a[5] * a[7]) * inv_det,
        (a[2] * a[7] - a[1] * a[8]) * inv_det,
        (a[1] * a[5] - a[2] * a[4]) * inv_det,
        (a[5] * a[6] - a[3] * a[8]) * inv_det,
        (a[0] * a[8] - a[2] * a[6]) * inv_det,
        (a[2] * a[3] - a[0] * a[5]) * inv_det,
        (a[3] * a[7] - a[4] * a[6]) * inv_det,
        (a[1] * a[6] - a[0] * a[7]) * inv_det,
        (a[0] * a[4] - a[1] * a[3]) * inv_det
      ])
    end

    def transpose
      Mat3.new([
        @data[0], @data[3], @data[6],
        @data[1], @data[4], @data[7],
        @data[2], @data[5], @data[8]
      ])
    end

    def adjoint
      a = @data
      Mat3.new([
        a[4] * a[8] - a[5] * a[7],
        a[2] * a[7] - a[1] * a[8],
        a[1] * a[5] - a[2] * a[4],
        a[5] * a[6] - a[3] * a[8],
        a[0] * a[8] - a[2] * a[6],
        a[2] * a[3] - a[0] * a[5],
        a[3] * a[7] - a[4] * a[6],
        a[1] * a[6] - a[0] * a[7],
        a[0] * a[4] - a[1] * a[3]
      ])
    end

    def frobenius_norm
      Math.sqrt(@data.sum { |v| v * v })
    end

    def translate(x, y)
      a = @data
      Mat3.new([
        a[0], a[1], a[2],
        a[3], a[4], a[5],
        x * a[0] + y * a[3] + a[6],
        x * a[1] + y * a[4] + a[7],
        x * a[2] + y * a[5] + a[8]
      ])
    end

    def rotate(radians)
      self * Mat3.rotation(radians)
    end

    def scale(x, y)
      a = @data
      Mat3.new([
        a[0] * x, a[1] * x, a[2] * x,
        a[3] * y, a[4] * y, a[5] * y,
        a[6], a[7], a[8]
      ])
    end

    def to_a
      @data.dup
    end

    def ==(other)
      return false unless other.is_a?(Mat3)

      @data == other.data
    end

    def near?(other, epsilon = 1e-6)
      @data.zip(other.data).all? { |a, b| (a - b).abs < epsilon }
    end

    def inspect
      "Mat3[\n" \
      "  #{@data[0..2].map { |v| format('%8.4f', v) }.join(', ')}\n" \
      "  #{@data[3..5].map { |v| format('%8.4f', v) }.join(', ')}\n" \
      "  #{@data[6..8].map { |v| format('%8.4f', v) }.join(', ')}\n]"
    end

    alias to_s inspect
  end
end
