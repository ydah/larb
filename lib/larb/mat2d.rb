# frozen_string_literal: true

module Larb
  # 2D affine transformation matrix (2x3)
  # Layout: [a, b, c, d, tx, ty]
  # Represents:
  # | a  c  tx |
  # | b  d  ty |
  # | 0  0  1  |
  class Mat2d
    attr_reader :data

    def initialize(data = nil)
      @data = data || [1.0, 0.0, 0.0, 1.0, 0.0, 0.0]
    end

    def self.identity
      new
    end

    def self.zero
      new([0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    end

    def self.translation(x, y)
      new([1.0, 0.0, 0.0, 1.0, x.to_f, y.to_f])
    end

    def self.rotation(radians)
      c = Math.cos(radians)
      s = Math.sin(radians)
      new([c, s, -s, c, 0.0, 0.0])
    end

    def self.scaling(x, y)
      new([x.to_f, 0.0, 0.0, y.to_f, 0.0, 0.0])
    end

    def self.from_rotation_translation_scale(rotation, translation, scale)
      c = Math.cos(rotation)
      s = Math.sin(rotation)
      new([
        c * scale.x,
        s * scale.x,
        -s * scale.y,
        c * scale.y,
        translation.x,
        translation.y
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
      when Mat2d
        a = @data
        b = other.data
        Mat2d.new([
          a[0] * b[0] + a[2] * b[1],
          a[1] * b[0] + a[3] * b[1],
          a[0] * b[2] + a[2] * b[3],
          a[1] * b[2] + a[3] * b[3],
          a[0] * b[4] + a[2] * b[5] + a[4],
          a[1] * b[4] + a[3] * b[5] + a[5]
        ])
      when Vec2
        Vec2.new(
          @data[0] * other.x + @data[2] * other.y + @data[4],
          @data[1] * other.x + @data[3] * other.y + @data[5]
        )
      when Numeric
        Mat2d.new(@data.map { |v| v * other })
      end
    end

    def +(other)
      Mat2d.new(@data.zip(other.data).map { |a, b| a + b })
    end

    def -(other)
      Mat2d.new(@data.zip(other.data).map { |a, b| a - b })
    end

    def determinant
      @data[0] * @data[3] - @data[1] * @data[2]
    end

    def inverse
      a = @data
      det = determinant
      raise "Matrix is not invertible" if det.abs < 1e-10

      inv_det = 1.0 / det
      Mat2d.new([
        a[3] * inv_det,
        -a[1] * inv_det,
        -a[2] * inv_det,
        a[0] * inv_det,
        (a[2] * a[5] - a[3] * a[4]) * inv_det,
        (a[1] * a[4] - a[0] * a[5]) * inv_det
      ])
    end

    def translate(x, y)
      a = @data
      Mat2d.new([
        a[0], a[1], a[2], a[3],
        a[0] * x + a[2] * y + a[4],
        a[1] * x + a[3] * y + a[5]
      ])
    end

    def rotate(radians)
      self * Mat2d.rotation(radians)
    end

    def scale(x, y)
      a = @data
      Mat2d.new([
        a[0] * x, a[1] * x,
        a[2] * y, a[3] * y,
        a[4], a[5]
      ])
    end

    def extract_translation
      Vec2.new(@data[4], @data[5])
    end

    def extract_rotation
      Math.atan2(@data[1], @data[0])
    end

    def extract_scale
      Vec2.new(
        Math.sqrt(@data[0] * @data[0] + @data[1] * @data[1]),
        Math.sqrt(@data[2] * @data[2] + @data[3] * @data[3])
      )
    end

    def frobenius_norm
      Math.sqrt(@data.sum { |v| v * v })
    end

    def to_mat3
      Mat3.new([
        @data[0], @data[1], 0,
        @data[2], @data[3], 0,
        @data[4], @data[5], 1
      ])
    end

    def to_a
      @data.dup
    end

    def ==(other)
      return false unless other.is_a?(Mat2d)

      @data == other.data
    end

    def near?(other, epsilon = 1e-6)
      @data.zip(other.data).all? { |a, b| (a - b).abs < epsilon }
    end

    def inspect
      "Mat2d[#{@data.map { |v| format('%.4f', v) }.join(', ')}]"
    end

    alias to_s inspect
  end
end
