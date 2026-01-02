# frozen_string_literal: true

Gem::Specification.new do |spec|
  spec.name = "larb"
  spec.version = "0.1.0"
  spec.authors = ["Yudai Takada"]
  spec.email = ["t.yudai92@gmail.com"]

  spec.summary = "Linear algebra library for 2D/3D graphics in Ruby"
  spec.description = "Larb provides vectors, matrices, quaternions, and Color classes for 2D/3D graphics and mathematical computations"
  spec.homepage = "https://github.com/ydah/larb"
  spec.license = "MIT"
  spec.required_ruby_version = ">= 3.1.0"

  spec.metadata["source_code_uri"] = spec.homepage
  spec.metadata["changelog_uri"] = "#{spec.homepage}/blob/main/CHANGELOG.md"

  gemspec = File.basename(__FILE__)
  spec.files = IO.popen(%w[git ls-files -z], chdir: __dir__, err: IO::NULL) do |ls|
    ls.readlines("\x0", chomp: true).reject do |f|
      (f == gemspec) ||
        f.start_with?(*%w[bin/ test/ spec/ features/ .git .github appveyor Gemfile examples/])
    end
  end
  spec.require_paths = ["lib"]
end
