require 'mkmf'
require 'numo/narray'

$LOAD_PATH.each do |lp|
  if File.exist?(File.join(lp, 'numo/numo/narray.h'))
    $INCFLAGS = "-I#{lp}/numo #{$INCFLAGS}"
    break
  end
end

unless have_header('numo/narray.h')
  puts 'numo/narray.h not found.'
  exit(1)
end

if RUBY_PLATFORM =~ /mswin|cygwin|mingw/
  $LOAD_PATH.each do |lp|
    if File.exist?(File.join(lp, 'numo/libnarray.a'))
      $LDFLAGS = "-L#{lp}/numo #{$LDFLAGS}"
      break
    end
  end
  unless have_library('narray', 'nary_new')
    puts 'libnarray.a not found.'
    exit(1)
  end
end

$LDFLAGS << ' -lstdc++ '

$srcs = Dir.glob("#{$srcdir}/*.c").map { |path| File.basename(path) }
$srcs.concat(%w[linear.cpp newton.cpp daxpy.c ddot.c dnrm2.c dscal.c])

$INCFLAGS << " -I$(srcdir)/liblinear"
$VPATH << "$(srcdir)/liblinear"
$VPATH << "$(srcdir)/liblinear/blas"

create_makefile('numo/liblinear/liblinearext')
