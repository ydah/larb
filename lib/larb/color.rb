# frozen_string_literal: true

module Larb
  class Color
    attr_accessor :r, :g, :b, :a

    def initialize(r = 0.0, g = 0.0, b = 0.0, a = 1.0)
      @r = r.to_f
      @g = g.to_f
      @b = b.to_f
      @a = a.to_f
    end

    def self.[](r, g, b, a = 1.0)
      new(r, g, b, a)
    end

    def self.rgb(r, g, b)
      new(r, g, b, 1.0)
    end

    def self.rgba(r, g, b, a)
      new(r, g, b, a)
    end

    def self.from_bytes(r, g, b, a = 255)
      new(r / 255.0, g / 255.0, b / 255.0, a / 255.0)
    end

    def self.from_hex(hex)
      hex = hex.delete("#")
      r = hex[0..1].to_i(16) / 255.0
      g = hex[2..3].to_i(16) / 255.0
      b = hex[4..5].to_i(16) / 255.0
      a = hex.length > 6 ? hex[6..7].to_i(16) / 255.0 : 1.0
      new(r, g, b, a)
    end

    def self.black
      new(0, 0, 0, 1)
    end

    def self.white
      new(1, 1, 1, 1)
    end

    def self.red
      new(1, 0, 0, 1)
    end

    def self.green
      new(0, 1, 0, 1)
    end

    def self.blue
      new(0, 0, 1, 1)
    end

    def self.yellow
      new(1, 1, 0, 1)
    end

    def self.cyan
      new(0, 1, 1, 1)
    end

    def self.magenta
      new(1, 0, 1, 1)
    end

    def self.transparent
      new(0, 0, 0, 0)
    end

    def +(other)
      Color.new(@r + other.r, @g + other.g, @b + other.b, @a + other.a)
    end

    def -(other)
      Color.new(@r - other.r, @g - other.g, @b - other.b, @a - other.a)
    end

    def *(scalar)
      case scalar
      when Numeric
        Color.new(@r * scalar, @g * scalar, @b * scalar, @a * scalar)
      when Color
        Color.new(@r * scalar.r, @g * scalar.g, @b * scalar.b, @a * scalar.a)
      end
    end

    def lerp(other, t)
      Color.new(
        @r + (other.r - @r) * t,
        @g + (other.g - @g) * t,
        @b + (other.b - @b) * t,
        @a + (other.a - @a) * t
      )
    end

    def clamp
      Color.new(
        @r.clamp(0.0, 1.0),
        @g.clamp(0.0, 1.0),
        @b.clamp(0.0, 1.0),
        @a.clamp(0.0, 1.0)
      )
    end

    def to_bytes
      [(@r * 255).round.clamp(0, 255),
       (@g * 255).round.clamp(0, 255),
       (@b * 255).round.clamp(0, 255),
       (@a * 255).round.clamp(0, 255)]
    end

    def to_hex
      r, g, b, a = to_bytes
      format("#%02x%02x%02x%02x", r, g, b, a)
    end

    def to_vec3
      Vec3.new(@r, @g, @b)
    end

    def to_vec4
      Vec4.new(@r, @g, @b, @a)
    end

    def to_a
      [@r, @g, @b, @a]
    end

    def self.from_vec4(v)
      new(v.x, v.y, v.z, v.w)
    end

    def self.from_vec3(v, a = 1.0)
      new(v.x, v.y, v.z, a)
    end

    def inspect
      "Color[#{@r}, #{@g}, #{@b}, #{@a}]"
    end

    alias to_s inspect
  end
end
