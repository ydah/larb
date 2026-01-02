# frozen_string_literal: true

module Larb
  # Dual Quaternion for rigid body transformations
  # Represented as 8 values: [real quaternion (4), dual quaternion (4)]
  # real: rotation, dual: translation encoded
  class Quat2
    attr_reader :data

    def initialize(data = nil)
      @data = data || [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0]
    end

    def self.identity
      new
    end

    def self.from_rotation_translation(rotation, translation)
      rx, ry, rz, rw = rotation.x, rotation.y, rotation.z, rotation.w
      tx, ty, tz = translation.x, translation.y, translation.z

      new([
        rx, ry, rz, rw,
        (tx * rw + ty * rz - tz * ry) * 0.5,
        (ty * rw + tz * rx - tx * rz) * 0.5,
        (tz * rw + tx * ry - ty * rx) * 0.5,
        (-tx * rx - ty * ry - tz * rz) * 0.5
      ])
    end

    def self.from_translation(translation)
      from_rotation_translation(Quat.identity, translation)
    end

    def self.from_rotation(rotation)
      new([rotation.x, rotation.y, rotation.z, rotation.w, 0, 0, 0, 0])
    end

    def self.from_mat4(m)
      rotation = m.extract_rotation
      translation = m.extract_translation
      from_rotation_translation(rotation, translation)
    end

    def real
      Quat.new(@data[0], @data[1], @data[2], @data[3])
    end

    def dual
      Quat.new(@data[4], @data[5], @data[6], @data[7])
    end

    def [](i)
      @data[i]
    end

    def []=(i, v)
      @data[i] = v.to_f
    end

    def *(other)
      case other
      when Quat2
        ax0, ay0, az0, aw0 = @data[0], @data[1], @data[2], @data[3]
        bx1, by1, bz1, bw1 = @data[4], @data[5], @data[6], @data[7]
        ax1, ay1, az1, aw1 = other.data[0], other.data[1], other.data[2], other.data[3]
        bx0, by0, bz0, bw0 = other.data[4], other.data[5], other.data[6], other.data[7]

        Quat2.new([
          ax0 * aw1 + aw0 * ax1 + ay0 * az1 - az0 * ay1,
          ay0 * aw1 + aw0 * ay1 + az0 * ax1 - ax0 * az1,
          az0 * aw1 + aw0 * az1 + ax0 * ay1 - ay0 * ax1,
          aw0 * aw1 - ax0 * ax1 - ay0 * ay1 - az0 * az1,
          ax0 * bw0 + aw0 * bx0 + ay0 * bz0 - az0 * by0 +
            bx1 * aw1 + bw1 * ax1 + by1 * az1 - bz1 * ay1,
          ay0 * bw0 + aw0 * by0 + az0 * bx0 - ax0 * bz0 +
            by1 * aw1 + bw1 * ay1 + bz1 * ax1 - bx1 * az1,
          az0 * bw0 + aw0 * bz0 + ax0 * by0 - ay0 * bx0 +
            bz1 * aw1 + bw1 * az1 + bx1 * ay1 - by1 * ax1,
          aw0 * bw0 - ax0 * bx0 - ay0 * by0 - az0 * bz0 +
            bw1 * aw1 - bx1 * ax1 - by1 * ay1 - bz1 * az1
        ])
      when Vec3
        transform_point(other)
      when Numeric
        Quat2.new(@data.map { |v| v * other })
      end
    end

    def +(other)
      Quat2.new(@data.zip(other.data).map { |a, b| a + b })
    end

    def -(other)
      Quat2.new(@data.zip(other.data).map { |a, b| a - b })
    end

    def dot(other)
      @data[0] * other.data[0] + @data[1] * other.data[1] +
        @data[2] * other.data[2] + @data[3] * other.data[3]
    end

    def length
      Math.sqrt(length_squared)
    end

    def length_squared
      @data[0..3].sum { |v| v * v }
    end

    def normalize
      len = length
      return Quat2.new(@data.dup) if len < 1e-10

      inv_len = 1.0 / len
      Quat2.new(@data.map { |v| v * inv_len })
    end

    def normalize!
      len = length
      return self if len < 1e-10

      inv_len = 1.0 / len
      @data.map! { |v| v * inv_len }
      self
    end

    def conjugate
      Quat2.new([
        -@data[0], -@data[1], -@data[2], @data[3],
        -@data[4], -@data[5], -@data[6], @data[7]
      ])
    end

    def inverse
      len_sq = length_squared
      return conjugate if len_sq < 1e-10

      inv_len_sq = 1.0 / len_sq
      conj = conjugate
      Quat2.new(conj.data.map { |v| v * inv_len_sq })
    end

    def translation
      ax, ay, az, aw = @data[0], @data[1], @data[2], @data[3]
      bx, by, bz, bw = @data[4], @data[5], @data[6], @data[7]
      # t = 2 * dual * conjugate(real)
      Vec3.new(
        2 * (-bw * ax + bx * aw - by * az + bz * ay),
        2 * (-bw * ay + by * aw - bz * ax + bx * az),
        2 * (-bw * az + bz * aw - bx * ay + by * ax)
      )
    end

    def rotation
      real.normalize
    end

    def transform_point(point)
      rotation * point + translation
    end

    def lerp(other, t)
      Quat2.new(@data.zip(other.data).map { |a, b| a + (b - a) * t }).normalize
    end

    def to_mat4
      rot = rotation
      trans = translation
      Mat4.from_quaternion(rot) * Mat4.translation(trans.x, trans.y, trans.z)
    end

    def to_a
      @data.dup
    end

    def ==(other)
      return false unless other.is_a?(Quat2)

      @data == other.data
    end

    def near?(other, epsilon = 1e-6)
      @data.zip(other.data).all? { |a, b| (a - b).abs < epsilon }
    end

    def inspect
      "Quat2[real: #{real.inspect}, dual: #{dual.inspect}]"
    end

    alias to_s inspect
  end
end
