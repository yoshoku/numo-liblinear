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
  spec.description   = <<~MSG
    Numo::Liblinear is a Ruby gem binding to the LIBLINEAR library.
    LIBLINEAR is one of the famous libraries for large-scale regularized linear classification and regression.
    Numo::Liblinear makes to use the LIBLINEAR functions with dataset represented by Numo::NArray.
  MSG
  spec.homepage      = 'https://github.com/yoshoku/numo-liblinear'
  spec.license       = 'BSD-3-Clause'

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files         = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  end

  gem_dir = File.expand_path(__dir__) + '/'
  submodule_path = `git submodule --quiet foreach pwd`.split($OUTPUT_RECORD_SEPARATOR).first
  submodule_relative_path = submodule_path.sub gem_dir, ''
  liblinear_files = %w[linear.cpp linear.h newton.cpp newton.h blas/blas.h blas/blasp.h blas/daxpy.c blas/ddot.c blas/dnrm2.c blas/dscal.c]
  liblinear_files.each { |liblinf| spec.files << "#{submodule_relative_path}/#{liblinf}" }

  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions    = ['ext/numo/liblinear/extconf.rb']

  spec.metadata      = {
    'homepage_uri' => 'https://github.com/yoshoku/numo-liblinear',
    'source_code_uri' => 'https://github.com/yoshoku/numo-liblinear',
    'documentation_uri' => 'https://yoshoku.github.io/numo-liblinear/doc/'
  }

  spec.add_runtime_dependency 'numo-narray', '~> 0.9.1'

  spec.add_development_dependency 'bundler', '~> 2.0'
  spec.add_development_dependency 'rake', '~> 13.0'
  spec.add_development_dependency 'rake-compiler', '~> 1.0'
  spec.add_development_dependency 'rspec', '~> 3.0'
end
