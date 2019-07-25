#ifndef NUMO_LIBLINEAR_PARAMETER_H
#define NUMO_LIBLINEAR_PARAMETER_H 1

#include <linear.h>
#include <ruby.h>
#include <numo/narray.h>
#include <numo/template.h>

#include "converter.h"

struct parameter* rb_hash_to_parameter(VALUE parm_hash);
VALUE parameter_to_rb_hash(struct parameter* const param);
void xfree_parameter(struct parameter* param);

#endif /* NUMO_LIBLINEAR_PARAMETER_H */
