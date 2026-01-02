# frozen_string_literal: true

module Larb
  class Mat2
    attr_reader :data

    def initialize(data = nil)
      @data = data || [1.0, 0.0, 0.0, 1.0]
    end

    def self.identity
      new
    end

    def self.zero
      new([0.0, 0.0, 0.0, 0.0])
    end

    def self.rotation(radians)
      c = Math.cos(radians)
      s = Math.sin(radians)
      new([c, s, -s, c])
    end

    def self.scaling(x, y)
      new([x.to_f, 0.0, 0.0, y.to_f])
    end

    def self.from_vec2(v1, v2)
      new([v1.x, v1.y, v2.x, v2.y])
    end

    def [](i)
      @data[i]
    end

    def []=(i, v)
      @data[i] = v.to_f
    end

    def *(other)
      case other
      when Mat2
        a = @data
        b = other.data
        Mat2.new([
          a[0] * b[0] + a[2] * b[1],
          a[1] * b[0] + a[3] * b[1],
          a[0] * b[2] + a[2] * b[3],
          a[1] * b[2] + a[3] * b[3]
        ])
      when Vec2
        Vec2.new(
          @data[0] * other.x + @data[2] * other.y,
          @data[1] * other.x + @data[3] * other.y
        )
      when Numeric
        Mat2.new(@data.map { |v| v * other })
      end
    end

    def +(other)
      Mat2.new(@data.zip(other.data).map { |a, b| a + b })
    end

    def -(other)
      Mat2.new(@data.zip(other.data).map { |a, b| a - b })
    end

    def determinant
      @data[0] * @data[3] - @data[2] * @data[1]
    end

    def inverse
      det = determinant
      raise "Matrix is not invertible" if det.abs < 1e-10

      inv_det = 1.0 / det
      Mat2.new([
        @data[3] * inv_det,
        -@data[1] * inv_det,
        -@data[2] * inv_det,
        @data[0] * inv_det
      ])
    end

    def transpose
      Mat2.new([@data[0], @data[2], @data[1], @data[3]])
    end

    def adjoint
      Mat2.new([@data[3], -@data[1], -@data[2], @data[0]])
    end

    def frobenius_norm
      Math.sqrt(@data.sum { |v| v * v })
    end

    def to_a
      @data.dup
    end

    def ==(other)
      return false unless other.is_a?(Mat2)

      @data == other.data
    end

    def near?(other, epsilon = 1e-6)
      @data.zip(other.data).all? { |a, b| (a - b).abs < epsilon }
    end

    def inspect
      "Mat2[#{@data[0]}, #{@data[1]}, #{@data[2]}, #{@data[3]}]"
    end

    alias to_s inspect
  end
end
