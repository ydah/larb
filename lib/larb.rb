# frozen_string_literal: true

begin
  require "larb/larb"
rescue LoadError
  # Fallback to the Ruby implementation when the extension isn't built yet.
end

require_relative "larb/vec2" unless defined?(Larb::Vec2)
require_relative "larb/vec3" unless defined?(Larb::Vec3)
require_relative "larb/vec4" unless defined?(Larb::Vec4)
require_relative "larb/quat" unless defined?(Larb::Quat)
require_relative "larb/quat2" unless defined?(Larb::Quat2)
require_relative "larb/mat2" unless defined?(Larb::Mat2)
require_relative "larb/mat2d" unless defined?(Larb::Mat2d)
require_relative "larb/mat3" unless defined?(Larb::Mat3)
require_relative "larb/mat4" unless defined?(Larb::Mat4)
require_relative "larb/color" unless defined?(Larb::Color)

module Larb
  VERSION = "0.1.0"
end
