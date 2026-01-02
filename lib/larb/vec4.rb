# frozen_string_literal: true

module Larb
  class Vec4
    attr_accessor :x, :y, :z, :w

    def initialize(x = 0.0, y = 0.0, z = 0.0, w = 1.0)
      @x = x.to_f
      @y = y.to_f
      @z = z.to_f
      @w = w.to_f
    end

    def self.[](x, y, z, w = 1.0)
      new(x, y, z, w)
    end

    def self.zero
      new(0, 0, 0, 0)
    end

    def self.one
      new(1, 1, 1, 1)
    end

    def +(other)
      Vec4.new(@x + other.x, @y + other.y, @z + other.z, @w + other.w)
    end

    def -(other)
      Vec4.new(@x - other.x, @y - other.y, @z - other.z, @w - other.w)
    end

    def *(scalar)
      Vec4.new(@x * scalar, @y * scalar, @z * scalar, @w * scalar)
    end

    def /(scalar)
      Vec4.new(@x / scalar, @y / scalar, @z / scalar, @w / scalar)
    end

    def -@
      Vec4.new(-@x, -@y, -@z, -@w)
    end

    def dot(other)
      @x * other.x + @y * other.y + @z * other.z + @w * other.w
    end

    def length
      Math.sqrt(length_squared)
    end

    def length_squared
      @x * @x + @y * @y + @z * @z + @w * @w
    end

    def normalize
      self / length
    end

    def normalize!
      l = length
      @x /= l
      @y /= l
      @z /= l
      @w /= l
      self
    end

    def perspective_divide
      return Vec3.new(@x, @y, @z) if @w == 0 || @w == 1

      Vec3.new(@x / @w, @y / @w, @z / @w)
    end

    def xyz
      Vec3.new(@x, @y, @z)
    end

    def xy
      Vec2.new(@x, @y)
    end

    def rgb
      Vec3.new(@x, @y, @z)
    end

    def to_a
      [@x, @y, @z, @w]
    end

    def [](i)
      to_a[i]
    end

    def ==(other)
      return false unless other.is_a?(Vec4)

      @x == other.x && @y == other.y && @z == other.z && @w == other.w
    end

    def near?(other, epsilon = 1e-6)
      (@x - other.x).abs < epsilon &&
        (@y - other.y).abs < epsilon &&
        (@z - other.z).abs < epsilon &&
        (@w - other.w).abs < epsilon
    end

    def lerp(other, t)
      Vec4.new(
        @x + (other.x - @x) * t,
        @y + (other.y - @y) * t,
        @z + (other.z - @z) * t,
        @w + (other.w - @w) * t
      )
    end

    def inspect
      "Vec4[#{@x}, #{@y}, #{@z}, #{@w}]"
    end

    alias to_s inspect
  end
end
