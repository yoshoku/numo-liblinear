require 'bundler/gem_tasks'
require 'rspec/core/rake_task'

RSpec::Core::RakeTask.new(:spec)

require 'rake/extensiontask'

task :build => :compile

Rake::ExtensionTask.new('liblinearext') do |ext|
  ext.ext_dir = 'ext/numo/liblinear'
  ext.lib_dir = 'lib/numo/liblinear'
end

task :default => [:clobber, :compile, :spec]
