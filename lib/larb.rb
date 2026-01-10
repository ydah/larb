# frozen_string_literal: true

begin
  require "larb/larb"
rescue LoadError
  # Fallback to the Ruby implementation when the extension isn't built yet.
end

unless defined?(Larb::Vec2)
  require_relative "larb/vec2"
  require_relative "larb/vec3"
  require_relative "larb/vec4"
  require_relative "larb/quat"
  require_relative "larb/quat2"
  require_relative "larb/mat2"
  require_relative "larb/mat2d"
  require_relative "larb/mat3"
  require_relative "larb/mat4"
  require_relative "larb/color"
end

module Larb
  VERSION = "0.1.0"
end
