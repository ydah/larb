# frozen_string_literal: true

module Larb
  class Mat4
    attr_reader :data

    def initialize(data = nil)
      @data = data || Array.new(16, 0.0)
      unless data
        @data[0] = @data[5] = @data[10] = @data[15] = 1.0
      end
    end

    def self.identity
      new
    end

    def self.zero
      new(Array.new(16, 0.0))
    end

    def self.translation(x, y, z)
      m = identity
      m[12] = x.to_f
      m[13] = y.to_f
      m[14] = z.to_f
      m
    end

    def self.scaling(x, y, z)
      m = zero
      m[0] = x.to_f
      m[5] = y.to_f
      m[10] = z.to_f
      m[15] = 1.0
      m
    end

    def self.rotation_x(radians)
      c = Math.cos(radians)
      s = Math.sin(radians)
      m = identity
      m[5] = c
      m[6] = s
      m[9] = -s
      m[10] = c
      m
    end

    def self.rotation_y(radians)
      c = Math.cos(radians)
      s = Math.sin(radians)
      m = identity
      m[0] = c
      m[2] = -s
      m[8] = s
      m[10] = c
      m
    end

    def self.rotation_z(radians)
      c = Math.cos(radians)
      s = Math.sin(radians)
      m = identity
      m[0] = c
      m[1] = s
      m[4] = -s
      m[5] = c
      m
    end

    def self.rotation(axis, radians)
      axis = axis.normalize
      c = Math.cos(radians)
      s = Math.sin(radians)
      t = 1 - c
      x = axis.x
      y = axis.y
      z = axis.z

      new([
        t * x * x + c,     t * x * y + s * z, t * x * z - s * y, 0,
        t * x * y - s * z, t * y * y + c,     t * y * z + s * x, 0,
        t * x * z + s * y, t * y * z - s * x, t * z * z + c,     0,
        0,                 0,                 0,                 1
      ])
    end

    def self.look_at(eye, target, up)
      f = (target - eye).normalize
      r = f.cross(up).normalize
      u = r.cross(f)

      new([
        r.x,         u.x,         -f.x,        0,
        r.y,         u.y,         -f.y,        0,
        r.z,         u.z,         -f.z,        0,
        -r.dot(eye), -u.dot(eye), f.dot(eye),  1
      ])
    end

    def self.perspective(fov_y, aspect, near, far)
      f = 1.0 / Math.tan(fov_y / 2.0)
      nf = 1.0 / (near - far)

      new([
        f / aspect, 0, 0,                      0,
        0,          f, 0,                      0,
        0,          0, (far + near) * nf,     -1,
        0,          0, 2 * far * near * nf,    0
      ])
    end

    def self.orthographic(left, right, bottom, top, near, far)
      rl = 1.0 / (right - left)
      tb = 1.0 / (top - bottom)
      fn = 1.0 / (far - near)

      new([
        2 * rl,                  0,                       0,                       0,
        0,                       2 * tb,                  0,                       0,
        0,                       0,                      -2 * fn,                  0,
        -(right + left) * rl,   -(top + bottom) * tb,   -(far + near) * fn,       1
      ])
    end

    def self.frustum(left, right, bottom, top, near, far)
      rl = 1.0 / (right - left)
      tb = 1.0 / (top - bottom)
      nf = 1.0 / (near - far)

      new([
        2 * near * rl,           0,                       0,                       0,
        0,                       2 * near * tb,           0,                       0,
        (right + left) * rl,    (top + bottom) * tb,     (far + near) * nf,       -1,
        0,                       0,                       2 * far * near * nf,     0
      ])
    end

    def self.from_quaternion(q)
      x, y, z, w = q.x, q.y, q.z, q.w

      x2 = x + x
      y2 = y + y
      z2 = z + z

      xx = x * x2
      xy = x * y2
      xz = x * z2
      yy = y * y2
      yz = y * z2
      zz = z * z2
      wx = w * x2
      wy = w * y2
      wz = w * z2

      new([
        1 - (yy + zz), xy + wz,       xz - wy,       0,
        xy - wz,       1 - (xx + zz), yz + wx,       0,
        xz + wy,       yz - wx,       1 - (xx + yy), 0,
        0,             0,             0,             1
      ])
    end

    def self.trs(translation, rotation, scale)
      from_quaternion(rotation) * scaling(scale.x, scale.y, scale.z) *
        self.translation(translation.x, translation.y, translation.z)
    end

    def [](i)
      @data[i]
    end

    def []=(i, v)
      @data[i] = v.to_f
    end

    def *(other)
      case other
      when Mat4
        result = Array.new(16, 0.0)
        4.times do |col|
          4.times do |row|
            4.times do |k|
              result[col * 4 + row] += @data[k * 4 + row] * other[col * 4 + k]
            end
          end
        end
        Mat4.new(result)

      when Vec4
        Vec4.new(
          @data[0] * other.x + @data[4] * other.y + @data[8] * other.z + @data[12] * other.w,
          @data[1] * other.x + @data[5] * other.y + @data[9] * other.z + @data[13] * other.w,
          @data[2] * other.x + @data[6] * other.y + @data[10] * other.z + @data[14] * other.w,
          @data[3] * other.x + @data[7] * other.y + @data[11] * other.z + @data[15] * other.w
        )

      when Vec3
        self * other.to_vec4
      end
    end

    def transpose
      Mat4.new([
        @data[0], @data[4], @data[8],  @data[12],
        @data[1], @data[5], @data[9],  @data[13],
        @data[2], @data[6], @data[10], @data[14],
        @data[3], @data[7], @data[11], @data[15]
      ])
    end

    def inverse
      m = @data
      inv = Array.new(16)

      inv[0]  =  m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10]
      inv[4]  = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10]
      inv[8]  =  m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9]
      inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9]

      inv[1]  = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10]
      inv[5]  =  m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10]
      inv[9]  = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9]
      inv[13] =  m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9]

      inv[2]  =  m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6]
      inv[6]  = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6]
      inv[10] =  m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5]
      inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5]

      inv[3]  = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6]
      inv[7]  =  m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6]
      inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5]
      inv[15] =  m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5]

      det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12]
      raise "Matrix is not invertible" if det.abs < 1e-10

      det = 1.0 / det
      Mat4.new(inv.map { |v| v * det })
    end

    def to_a
      @data.dup
    end

    def determinant
      m = @data
      m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) -
              m[9] * (m[6] * m[15] - m[7] * m[14]) +
              m[13] * (m[6] * m[11] - m[7] * m[10])) -
        m[4] * (m[1] * (m[10] * m[15] - m[11] * m[14]) -
                m[9] * (m[2] * m[15] - m[3] * m[14]) +
                m[13] * (m[2] * m[11] - m[3] * m[10])) +
        m[8] * (m[1] * (m[6] * m[15] - m[7] * m[14]) -
                m[5] * (m[2] * m[15] - m[3] * m[14]) +
                m[13] * (m[2] * m[7] - m[3] * m[6])) -
        m[12] * (m[1] * (m[6] * m[11] - m[7] * m[10]) -
                 m[5] * (m[2] * m[11] - m[3] * m[10]) +
                 m[9] * (m[2] * m[7] - m[3] * m[6]))
    end

    def +(other)
      Mat4.new(@data.zip(other.data).map { |a, b| a + b })
    end

    def -(other)
      Mat4.new(@data.zip(other.data).map { |a, b| a - b })
    end

    def ==(other)
      return false unless other.is_a?(Mat4)

      @data == other.data
    end

    def near?(other, epsilon = 1e-6)
      @data.zip(other.data).all? { |a, b| (a - b).abs < epsilon }
    end

    def extract_translation
      Vec3.new(@data[12], @data[13], @data[14])
    end

    def extract_scale
      sx = Vec3.new(@data[0], @data[1], @data[2]).length
      sy = Vec3.new(@data[4], @data[5], @data[6]).length
      sz = Vec3.new(@data[8], @data[9], @data[10]).length
      Vec3.new(sx, sy, sz)
    end

    def extract_rotation
      scale = extract_scale
      m00 = @data[0] / scale.x
      m01 = @data[1] / scale.x
      m02 = @data[2] / scale.x
      m10 = @data[4] / scale.y
      m11 = @data[5] / scale.y
      m12 = @data[6] / scale.y
      m20 = @data[8] / scale.z
      m21 = @data[9] / scale.z
      m22 = @data[10] / scale.z

      trace = m00 + m11 + m22
      if trace > 0
        s = 0.5 / Math.sqrt(trace + 1.0)
        Quat.new((m12 - m21) * s, (m20 - m02) * s, (m01 - m10) * s, 0.25 / s)
      elsif m00 > m11 && m00 > m22
        s = 2.0 * Math.sqrt(1.0 + m00 - m11 - m22)
        Quat.new(0.25 * s, (m10 + m01) / s, (m20 + m02) / s, (m12 - m21) / s)
      elsif m11 > m22
        s = 2.0 * Math.sqrt(1.0 + m11 - m00 - m22)
        Quat.new((m10 + m01) / s, 0.25 * s, (m21 + m12) / s, (m20 - m02) / s)
      else
        s = 2.0 * Math.sqrt(1.0 + m22 - m00 - m11)
        Quat.new((m20 + m02) / s, (m21 + m12) / s, 0.25 * s, (m01 - m10) / s)
      end
    end

    def inspect
      "Mat4[\n" \
      "  #{@data[0..3].map { |v| format('%8.4f', v) }.join(', ')}\n" \
      "  #{@data[4..7].map { |v| format('%8.4f', v) }.join(', ')}\n" \
      "  #{@data[8..11].map { |v| format('%8.4f', v) }.join(', ')}\n" \
      "  #{@data[12..15].map { |v| format('%8.4f', v) }.join(', ')}\n]"
    end
  end
end
