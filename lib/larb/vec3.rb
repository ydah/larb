# frozen_string_literal: true

module Larb
  class Vec3
    attr_accessor :x, :y, :z

    def initialize(x = 0.0, y = 0.0, z = 0.0)
      @x = x.to_f
      @y = y.to_f
      @z = z.to_f
    end

    def self.[](x, y, z)
      new(x, y, z)
    end

    def self.zero
      new(0, 0, 0)
    end

    def self.one
      new(1, 1, 1)
    end

    def self.up
      new(0, 1, 0)
    end

    def self.down
      new(0, -1, 0)
    end

    def self.forward
      new(0, 0, -1)
    end

    def self.back
      new(0, 0, 1)
    end

    def self.right
      new(1, 0, 0)
    end

    def self.left
      new(-1, 0, 0)
    end

    def +(other)
      Vec3.new(@x + other.x, @y + other.y, @z + other.z)
    end

    def -(other)
      Vec3.new(@x - other.x, @y - other.y, @z - other.z)
    end

    def *(scalar)
      case scalar
      when Numeric
        Vec3.new(@x * scalar, @y * scalar, @z * scalar)
      when Vec3
        Vec3.new(@x * scalar.x, @y * scalar.y, @z * scalar.z)
      end
    end

    def /(scalar)
      Vec3.new(@x / scalar, @y / scalar, @z / scalar)
    end

    def -@
      Vec3.new(-@x, -@y, -@z)
    end

    def dot(other)
      @x * other.x + @y * other.y + @z * other.z
    end

    def cross(other)
      Vec3.new(
        @y * other.z - @z * other.y,
        @z * other.x - @x * other.z,
        @x * other.y - @y * other.x
      )
    end

    def length
      Math.sqrt(@x * @x + @y * @y + @z * @z)
    end

    def length_squared
      @x * @x + @y * @y + @z * @z
    end

    def normalize
      self / length
    end

    def reflect(normal)
      self - normal * (2 * dot(normal))
    end

    def xy
      Vec2.new(@x, @y)
    end

    def xz
      Vec2.new(@x, @z)
    end

    def yz
      Vec2.new(@y, @z)
    end

    def lerp(other, t)
      self + (other - self) * t
    end

    def to_a
      [@x, @y, @z]
    end

    def to_vec4(w = 1.0)
      Vec4.new(@x, @y, @z, w)
    end

    def [](i)
      to_a[i]
    end

    def ==(other)
      return false unless other.is_a?(Vec3)

      @x == other.x && @y == other.y && @z == other.z
    end

    def near?(other, epsilon = 1e-6)
      (@x - other.x).abs < epsilon &&
        (@y - other.y).abs < epsilon &&
        (@z - other.z).abs < epsilon
    end

    def distance(other)
      Math.sqrt(distance_squared(other))
    end

    def distance_squared(other)
      dx = @x - other.x
      dy = @y - other.y
      dz = @z - other.z
      dx * dx + dy * dy + dz * dz
    end

    def angle_between(other)
      d = dot(other) / (length * other.length)
      Math.acos(d.clamp(-1.0, 1.0))
    end

    def project(onto)
      onto * (dot(onto) / onto.length_squared)
    end

    def reject(from)
      self - project(from)
    end

    def slerp(other, t)
      dot_val = normalize.dot(other.normalize).clamp(-1.0, 1.0)
      theta = Math.acos(dot_val) * t
      relative = (other - self * dot_val).normalize
      self * Math.cos(theta) + relative * Math.sin(theta)
    end

    def clamp_length(max_length)
      len_sq = length_squared
      return self if len_sq <= max_length * max_length

      self * (max_length / Math.sqrt(len_sq))
    end

    def normalize!
      l = length
      @x /= l
      @y /= l
      @z /= l
      self
    end

    def min(other)
      Vec3.new([@x, other.x].min, [@y, other.y].min, [@z, other.z].min)
    end

    def max(other)
      Vec3.new([@x, other.x].max, [@y, other.y].max, [@z, other.z].max)
    end

    def abs
      Vec3.new(@x.abs, @y.abs, @z.abs)
    end

    def floor
      Vec3.new(@x.floor, @y.floor, @z.floor)
    end

    def ceil
      Vec3.new(@x.ceil, @y.ceil, @z.ceil)
    end

    def round
      Vec3.new(@x.round, @y.round, @z.round)
    end

    def inspect
      "Vec3[#{@x}, #{@y}, #{@z}]"
    end

    alias to_s inspect
  end
end
