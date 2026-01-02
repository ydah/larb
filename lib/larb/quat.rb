# frozen_string_literal: true

module Larb
  class Quat
    attr_accessor :x, :y, :z, :w

    def initialize(x = 0.0, y = 0.0, z = 0.0, w = 1.0)
      @x = x.to_f
      @y = y.to_f
      @z = z.to_f
      @w = w.to_f
    end

    def self.[](x, y, z, w)
      new(x, y, z, w)
    end

    def self.identity
      new(0, 0, 0, 1)
    end

    def self.from_axis_angle(axis, radians)
      half = radians * 0.5
      s = Math.sin(half)
      axis = axis.normalize
      new(axis.x * s, axis.y * s, axis.z * s, Math.cos(half))
    end

    def self.from_euler(x, y, z)
      cx = Math.cos(x * 0.5)
      sx = Math.sin(x * 0.5)
      cy = Math.cos(y * 0.5)
      sy = Math.sin(y * 0.5)
      cz = Math.cos(z * 0.5)
      sz = Math.sin(z * 0.5)

      new(
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
      )
    end

    def self.look_rotation(forward, up = Vec3.up)
      forward = forward.normalize
      right = up.cross(forward).normalize
      up = forward.cross(right)

      m00 = right.x
      m01 = up.x
      m02 = forward.x
      m10 = right.y
      m11 = up.y
      m12 = forward.y
      m20 = right.z
      m21 = up.z
      m22 = forward.z

      trace = m00 + m11 + m22
      if trace > 0
        s = 0.5 / Math.sqrt(trace + 1.0)
        new((m21 - m12) * s, (m02 - m20) * s, (m10 - m01) * s, 0.25 / s)
      elsif m00 > m11 && m00 > m22
        s = 2.0 * Math.sqrt(1.0 + m00 - m11 - m22)
        new(0.25 * s, (m01 + m10) / s, (m02 + m20) / s, (m21 - m12) / s)
      elsif m11 > m22
        s = 2.0 * Math.sqrt(1.0 + m11 - m00 - m22)
        new((m01 + m10) / s, 0.25 * s, (m12 + m21) / s, (m02 - m20) / s)
      else
        s = 2.0 * Math.sqrt(1.0 + m22 - m00 - m11)
        new((m02 + m20) / s, (m12 + m21) / s, 0.25 * s, (m10 - m01) / s)
      end
    end

    def *(other)
      case other
      when Quat
        Quat.new(
          @w * other.x + @x * other.w + @y * other.z - @z * other.y,
          @w * other.y - @x * other.z + @y * other.w + @z * other.x,
          @w * other.z + @x * other.y - @y * other.x + @z * other.w,
          @w * other.w - @x * other.x - @y * other.y - @z * other.z
        )
      when Vec3
        qv = Vec3.new(@x, @y, @z)
        uv = qv.cross(other)
        uuv = qv.cross(uv)
        other + (uv * @w + uuv) * 2
      when Numeric
        Quat.new(@x * other, @y * other, @z * other, @w * other)
      end
    end

    def +(other)
      Quat.new(@x + other.x, @y + other.y, @z + other.z, @w + other.w)
    end

    def -(other)
      Quat.new(@x - other.x, @y - other.y, @z - other.z, @w - other.w)
    end

    def -@
      Quat.new(-@x, -@y, -@z, -@w)
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
      l = length
      Quat.new(@x / l, @y / l, @z / l, @w / l)
    end

    def normalize!
      l = length
      @x /= l
      @y /= l
      @z /= l
      @w /= l
      self
    end

    def conjugate
      Quat.new(-@x, -@y, -@z, @w)
    end

    def inverse
      len_sq = length_squared
      Quat.new(-@x / len_sq, -@y / len_sq, -@z / len_sq, @w / len_sq)
    end

    def lerp(other, t)
      Quat.new(
        @x + (other.x - @x) * t,
        @y + (other.y - @y) * t,
        @z + (other.z - @z) * t,
        @w + (other.w - @w) * t
      ).normalize
    end

    def slerp(other, t)
      dot_val = dot(other)

      other = -other if dot_val < 0
      dot_val = dot_val.abs

      if dot_val > 0.9995
        return lerp(other, t)
      end

      theta_0 = Math.acos(dot_val.clamp(-1.0, 1.0))
      theta = theta_0 * t
      sin_theta = Math.sin(theta)
      sin_theta_0 = Math.sin(theta_0)

      s0 = Math.cos(theta) - dot_val * sin_theta / sin_theta_0
      s1 = sin_theta / sin_theta_0

      Quat.new(
        @x * s0 + other.x * s1,
        @y * s0 + other.y * s1,
        @z * s0 + other.z * s1,
        @w * s0 + other.w * s1
      )
    end

    def to_axis_angle
      angle = 2 * Math.acos(@w.clamp(-1.0, 1.0))
      s = Math.sqrt(1 - @w * @w)
      if s < 0.001
        axis = Vec3.new(1, 0, 0)
      else
        axis = Vec3.new(@x / s, @y / s, @z / s)
      end
      [axis, angle]
    end

    def to_euler
      sinr_cosp = 2 * (@w * @x + @y * @z)
      cosr_cosp = 1 - 2 * (@x * @x + @y * @y)
      roll = Math.atan2(sinr_cosp, cosr_cosp)

      sinp = 2 * (@w * @y - @z * @x)
      pitch = if sinp.abs >= 1
                Math.copysign(Math::PI / 2, sinp)
              else
                Math.asin(sinp)
              end

      siny_cosp = 2 * (@w * @z + @x * @y)
      cosy_cosp = 1 - 2 * (@y * @y + @z * @z)
      yaw = Math.atan2(siny_cosp, cosy_cosp)

      Vec3.new(roll, pitch, yaw)
    end

    def to_mat4
      Mat4.from_quaternion(self)
    end

    def to_a
      [@x, @y, @z, @w]
    end

    def [](i)
      to_a[i]
    end

    def ==(other)
      return false unless other.is_a?(Quat)

      @x == other.x && @y == other.y && @z == other.z && @w == other.w
    end

    def near?(other, epsilon = 1e-6)
      (@x - other.x).abs < epsilon &&
        (@y - other.y).abs < epsilon &&
        (@z - other.z).abs < epsilon &&
        (@w - other.w).abs < epsilon
    end

    def inspect
      "Quat[#{@x}, #{@y}, #{@z}, #{@w}]"
    end

    alias to_s inspect
  end
end
