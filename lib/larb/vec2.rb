# frozen_string_literal: true

module Larb
  class Vec2
    attr_accessor :x, :y

    def initialize(x = 0.0, y = 0.0)
      @x = x.to_f
      @y = y.to_f
    end

    def self.[](x, y)
      new(x, y)
    end

    def self.zero
      new(0, 0)
    end

    def self.one
      new(1, 1)
    end

    def +(other)
      Vec2.new(@x + other.x, @y + other.y)
    end

    def -(other)
      Vec2.new(@x - other.x, @y - other.y)
    end

    def *(scalar)
      Vec2.new(@x * scalar, @y * scalar)
    end

    def /(scalar)
      Vec2.new(@x / scalar, @y / scalar)
    end

    def -@
      Vec2.new(-@x, -@y)
    end

    def dot(other)
      @x * other.x + @y * other.y
    end

    def length
      Math.sqrt(@x * @x + @y * @y)
    end

    def length_squared
      @x * @x + @y * @y
    end

    def normalize
      self / length
    end

    def normalize!
      l = length
      @x /= l
      @y /= l
      self
    end

    def lerp(other, t)
      self + (other - self) * t
    end

    def to_a
      [@x, @y]
    end

    def [](i)
      to_a[i]
    end

    def []=(i, v)
      if i == 0
        @x = v
      else
        @y = v
      end
    end

    def ==(other)
      return false unless other.is_a?(Vec2)

      @x == other.x && @y == other.y
    end

    def near?(other, epsilon = 1e-6)
      (@x - other.x).abs < epsilon && (@y - other.y).abs < epsilon
    end

    def angle
      Math.atan2(@y, @x)
    end

    def angle_to(other)
      Math.atan2(other.y - @y, other.x - @x)
    end

    def rotate(radians)
      c = Math.cos(radians)
      s = Math.sin(radians)
      Vec2.new(@x * c - @y * s, @x * s + @y * c)
    end

    def distance(other)
      Math.sqrt(distance_squared(other))
    end

    def distance_squared(other)
      dx = @x - other.x
      dy = @y - other.y
      dx * dx + dy * dy
    end

    def cross(other)
      @x * other.y - @y * other.x
    end

    def perpendicular
      Vec2.new(-@y, @x)
    end

    def reflect(normal)
      self - normal * (2 * dot(normal))
    end

    def clamp_length(max_length)
      len_sq = length_squared
      return self if len_sq <= max_length * max_length

      self * (max_length / Math.sqrt(len_sq))
    end

    def to_vec3(z = 0.0)
      Vec3.new(@x, @y, z)
    end

    def inspect
      "Vec2[#{@x}, #{@y}]"
    end

    alias to_s inspect
  end
end
