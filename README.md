# Larb [![Gem Version](https://badge.fury.io/rb/larb.svg)](https://badge.fury.io/rb/larb) [![CI](https://github.com/ydah/larb/actions/workflows/ci.yml/badge.svg)](https://github.com/ydah/larb/actions/workflows/ci.yml)

Linear algebra library for 2D/3D graphics in Ruby.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'larb'
```

And then execute:

```bash
bundle install
```

Or install it yourself as:

```bash
gem install larb
```

## Usage

```ruby
require 'larb'

# Vectors
v2 = Larb::Vec2.new(1, 2)
v3 = Larb::Vec3.new(1, 2, 3)
v4 = Larb::Vec4.new(1, 2, 3, 1)

# Vector operations
sum = v3 + Larb::Vec3.new(4, 5, 6)
dot = v3.dot(Larb::Vec3.new(1, 0, 0))
cross = v3.cross(Larb::Vec3.up)
normalized = v3.normalize

# Matrices
identity = Larb::Mat4.identity
translation = Larb::Mat4.translation(10, 20, 30)
rotation = Larb::Mat4.rotation_y(Math::PI / 4)
scale = Larb::Mat4.scaling(2, 2, 2)

# Matrix multiplication
transform = translation * rotation * scale
transformed_point = transform * v4

# Quaternions
quat = Larb::Quat.from_axis_angle(Larb::Vec3.up, Math::PI / 2)
rotated = quat * Larb::Vec3.new(1, 0, 0)

# Colors
red = Larb::Color.red
custom = Larb::Color.new(0.5, 0.3, 0.8, 1.0)
hex_color = Larb::Color.from_hex("#ff8800")
```

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).
