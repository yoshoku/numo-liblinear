# frozen_string_literal: true

lib = File.expand_path('lib', __dir__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'numo/liblinear/version'

Gem::Specification.new do |spec|
  spec.name          = 'numo-liblinear'
  spec.version       = Numo::Liblinear::VERSION
  spec.authors       = ['yoshoku']
  spec.email         = ['yoshoku@outlook.com']

  spec.summary       = <<~MSG
    Numo::Liblinear is a Ruby gem binding to the LIBLINEAR library.
    Numo::Liblinear makes to use the LIBLINEAR functions with dataset represented by Numo::NArray.
  MSG
  spec.description = <<~MSG
    Numo::Liblinear is a Ruby gem binding to the LIBLINEAR library.
    LIBLINEAR is one of the famous libraries for large-scale regularized linear classification and regression.
    Numo::Liblinear makes to use the LIBLINEAR functions with dataset represented by Numo::NArray.
  MSG
  spec.homepage      = 'https://github.com/yoshoku/numo-liblinear'
  spec.license       = 'BSD-3-Clause'

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files         = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features|sig-deps)/}) }
                     .select { |f| f.match(/\.(?:rb|rbs|h|hpp|c|cpp|md|txt)$/) }
  end
  spec.files << 'ext/numo/liblinear/src/COPYRIGHT'

  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions    = ['ext/numo/liblinear/extconf.rb']

  spec.metadata      = {
    'homepage_uri' => spec.homepage,
    'source_code_uri' => spec.homepage,
    'changelog_uri' => "#{spec.homepage}/blob/main/CHANGELOG.md",
    'documentation_uri' => "https://gemdocs.org/gems/#{spec.name}/#{spec.version}/",
    'rubygems_mfa_required' => 'true'
  }

  spec.add_dependency 'numo-narray-alt', '~> 0.9.3'
end
