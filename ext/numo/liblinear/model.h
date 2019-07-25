#ifndef NUMO_LIBLINEAR_MODEL_H
#define NUMO_LIBLINEAR_MODEL_H 1

#include <linear.h>
#include <ruby.h>
#include <numo/narray.h>
#include <numo/template.h>

#include "converter.h"

struct model* rb_hash_to_model(VALUE model_hash);
VALUE model_to_rb_hash(struct model* const model);
void xfree_model(struct model* model);

#endif /* NUMO_LIBLINEAR_MODEL_H */
