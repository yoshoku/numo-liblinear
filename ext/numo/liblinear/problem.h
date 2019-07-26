#ifndef NUMO_LIBLINEAR_PROBLEM_H
#define NUMO_LIBLINEAR_PROBLEM_H 1

#include <linear.h>
#include <ruby.h>
#include <numo/narray.h>
#include <numo/template.h>

void xfree_problem(struct problem* problem);
struct problem* dataset_to_problem(VALUE x_val, VALUE y_val);

#endif /* NUMO_LIBLINEAR_PROBLEM_H */
