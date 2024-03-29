require 'mkmf'
require 'numo/narray'

$LOAD_PATH.each do |lp|
  if File.exist?(File.join(lp, 'numo/numo/narray.h'))
    $INCFLAGS = "-I#{lp}/numo #{$INCFLAGS}"
    break
  end
end

abort 'numo/narray.h not found.' unless have_header('numo/narray.h')

if RUBY_PLATFORM =~ /mswin|cygwin|mingw/
  $LOAD_PATH.each do |lp|
    if File.exist?(File.join(lp, 'numo/libnarray.a'))
      $LDFLAGS = "-L#{lp}/numo #{$LDFLAGS}"
      break
    end
  end
  abort 'libnarray.a not found.' unless have_library('narray', 'nary_new')
end

if RUBY_PLATFORM.match?(/darwin/) && Gem::Version.new('3.1.0') <= Gem::Version.new(RUBY_VERSION)
  if try_link('int main(void){return 0;}', '-Wl,-undefined,dynamic_lookup')
    $LDFLAGS << ' -Wl,-undefined,dynamic_lookup'
  end
end

abort 'libstdc++ is not found.' unless have_library('stdc++')

$srcs = Dir.glob("#{$srcdir}/**/*.cpp").map { |path| File.basename(path) }
$srcs.concat(%w[daxpy.c ddot.c dnrm2.c dscal.c])

$INCFLAGS << " -I$(srcdir)/src"
$VPATH << "$(srcdir)/src"
$VPATH << "$(srcdir)/src/blas"

create_makefile('numo/liblinear/liblinearext')
