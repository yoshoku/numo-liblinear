/**
 * Copyright (c) 2019-2026 Atsushi Tatsuma
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBLINEAREXT_HPP
#define LIBLINEAREXT_HPP 1

#include <cmath>
#include <cstring>

#include <ruby.h>

#include <numo/narray.h>
#include <numo/template.h>

#include <linear.h>

typedef struct model LibLinearModel;
typedef struct feature_node LibLinearNode;
typedef struct parameter LibLinearParameter;
typedef struct problem LibLinearProblem;

void printNull(const char* s) {}

/** CONVERTERS */
VALUE convertVectorXiToNArray(const int* const arr, const int size) {
  size_t shape[1] = {(size_t)size};
  VALUE vec_val = rb_narray_new(numo_cInt32, 1, shape);
  int32_t* vec_ptr = (int32_t*)na_get_pointer_for_write(vec_val);
  for (int i = 0; i < size; i++) vec_ptr[i] = (int32_t)arr[i];
  return vec_val;
}

int* convertNArrayToVectorXi(VALUE vec_val) {
  if (NIL_P(vec_val)) return NULL;

  narray_t* vec_nary;
  GetNArray(vec_val, vec_nary);
  const size_t n_elements = NA_SHAPE(vec_nary)[0];

  int* arr = ALLOC_N(int, n_elements);
  const int32_t* const vec_pt = (int32_t*)na_get_pointer_for_read(vec_val);
  for (size_t i = 0; i < n_elements; i++) arr[i] = (int)vec_pt[i];

  RB_GC_GUARD(vec_val);

  return arr;
}

VALUE convertVectorXdToNArray(const double* const arr, const int size) {
  size_t shape[1] = {(size_t)size};
  VALUE vec_val = rb_narray_new(numo_cDFloat, 1, shape);
  double* vec_ptr = (double*)na_get_pointer_for_write(vec_val);
  memcpy(vec_ptr, arr, size * sizeof(double));
  return vec_val;
}

double* convertNArrayToVectorXd(VALUE vec_val) {
  if (NIL_P(vec_val)) return NULL;

  narray_t* vec_nary;
  GetNArray(vec_val, vec_nary);
  const size_t n_elements = NA_SHAPE(vec_nary)[0];
  double* arr = ALLOC_N(double, n_elements);
  const double* const vec_ptr = (double*)na_get_pointer_for_read(vec_val);
  memcpy(arr, vec_ptr, n_elements * sizeof(double));

  RB_GC_GUARD(vec_val);

  return arr;
}

VALUE convertMatrixXdToNArray(const double* const* mat, const int n_rows, const int n_cols) {
  size_t shape[2] = {(size_t)n_rows, (size_t)n_cols};
  VALUE mat_val = rb_narray_new(numo_cDFloat, 2, shape);
  double* mat_ptr = (double*)na_get_pointer_for_write(mat_val);
  for (int i = 0; i < n_rows; i++) memcpy(&mat_ptr[i * n_cols], mat[i], n_cols * sizeof(double));
  return mat_val;
}

double** convertNArrayToMatrixXd(VALUE mat_val) {
  if (NIL_P(mat_val)) return NULL;

  narray_t* mat_nary;
  GetNArray(mat_val, mat_nary);
  const size_t n_rows = NA_SHAPE(mat_nary)[0];
  const size_t n_cols = NA_SHAPE(mat_nary)[1];
  const double* const mat_ptr = (double*)na_get_pointer_for_read(mat_val);
  double** mat = ALLOC_N(double*, n_rows);
  for (size_t i = 0; i < n_rows; i++) {
    mat[i] = ALLOC_N(double, n_cols);
    memcpy(mat[i], &mat_ptr[i * n_cols], n_cols * sizeof(double));
  }

  RB_GC_GUARD(mat_val);

  return mat;
}

LibLinearNode* convertVectorXdToLibLinearNode(const double* const arr, const int size) {
  int n_nonzero_elements = 0;
  for (int i = 0; i < size; i++) {
    if (arr[i] != 0.0) n_nonzero_elements++;
  }

  LibLinearNode* node = ALLOC_N(LibLinearNode, n_nonzero_elements + 1);
  for (int i = 0, j = 0; i < size; i++) {
    if (arr[i] != 0.0) {
      node[j].index = i + 1;
      node[j].value = arr[i];
      j++;
    }
  }
  node[n_nonzero_elements].index = -1;
  node[n_nonzero_elements].value = 0.0;

  return node;
}

LibLinearModel* convertHashToLibLinearModel(VALUE model_hash) {
  LibLinearModel* model = ALLOC(LibLinearModel);
  VALUE el;
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("nr_class")));
  model->nr_class = !NIL_P(el) ? NUM2INT(el) : 0;
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("nr_feature")));
  model->nr_feature = !NIL_P(el) ? NUM2INT(el) : 0;
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("w")));
  model->w = convertNArrayToVectorXd(el);
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("label")));
  model->label = convertNArrayToVectorXi(el);
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("bias")));
  model->bias = NUM2DBL(el);
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("rho")));
  model->rho = NUM2DBL(el);
  return model;
}

VALUE convertLibLinearModelToHash(const LibLinearModel* const model) {
  const int n_cols = model->nr_class > 2 ? model->nr_class : 1;
  const int n_rows = model->nr_feature;
  VALUE model_hash = rb_hash_new();
  rb_hash_aset(model_hash, ID2SYM(rb_intern("nr_class")), INT2NUM(model->nr_class));
  rb_hash_aset(model_hash, ID2SYM(rb_intern("nr_feature")), INT2NUM(model->nr_feature));
  rb_hash_aset(model_hash, ID2SYM(rb_intern("w")), model->w ? convertVectorXdToNArray(model->w, n_rows * n_cols) : Qnil);
  rb_hash_aset(model_hash, ID2SYM(rb_intern("label")),
               model->label ? convertVectorXiToNArray(model->label, model->nr_class) : Qnil);
  rb_hash_aset(model_hash, ID2SYM(rb_intern("bias")), DBL2NUM(model->bias));
  rb_hash_aset(model_hash, ID2SYM(rb_intern("rho")), DBL2NUM(model->rho));
  return model_hash;
}

LibLinearParameter* convertHashToLibLinearParameter(VALUE param_hash) {
  LibLinearParameter* param = ALLOC(LibLinearParameter);
  VALUE el;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("solver_type")));
  param->solver_type = !NIL_P(el) ? NUM2INT(el) : L2R_L2LOSS_SVC_DUAL;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("eps")));
  if (!NIL_P(el)) {
    param->eps = NUM2DBL(el);
  } else {
    switch (param->solver_type) {
    case L2R_LR:
    case L2R_L2LOSS_SVC:
      param->eps = 0.01;
      break;
    case L2R_L2LOSS_SVR:
      param->eps = 0.0001;
      break;
    case L2R_L2LOSS_SVC_DUAL:
    case L2R_L1LOSS_SVC_DUAL:
    case MCSVM_CS:
    case L2R_LR_DUAL:
      param->eps = 0.1;
      break;
    case L1R_L2LOSS_SVC:
    case L1R_LR:
      param->eps = 0.01;
      break;
    case L2R_L1LOSS_SVR_DUAL:
    case L2R_L2LOSS_SVR_DUAL:
      param->eps = 0.1;
      break;
    case ONECLASS_SVM:
      param->eps = 0.01;
      break;
    }
  }
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("C")));
  param->C = !NIL_P(el) ? NUM2DBL(el) : 1;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("nr_weight")));
  param->nr_weight = !NIL_P(el) ? NUM2INT(el) : 0;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("weight_label")));
  param->weight_label = NULL;
  if (!NIL_P(el)) {
    param->weight_label = ALLOC_N(int, param->nr_weight);
    memcpy(param->weight_label, (int32_t*)na_get_pointer_for_read(el), param->nr_weight * sizeof(int32_t));
  }
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("weight")));
  param->weight = NULL;
  if (!NIL_P(el)) {
    param->weight = ALLOC_N(double, param->nr_weight);
    memcpy(param->weight, (double*)na_get_pointer_for_read(el), param->nr_weight * sizeof(double));
  }
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("p")));
  param->p = !NIL_P(el) ? NUM2DBL(el) : 0.1;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("nu")));
  param->nu = !NIL_P(el) ? NUM2DBL(el) : 0.5;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("init_sol")));
  param->init_sol = !NIL_P(el) ? convertNArrayToVectorXd(el) : NULL;
  param->regularize_bias = 1;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("w_recalc")));
  param->w_recalc = !NIL_P(el) ? (RTEST(el) ? true : false) : false;
  return param;
}

VALUE convertLibLinearParameterToHash(const LibLinearParameter* const param) {
  VALUE param_hash = rb_hash_new();
  rb_hash_aset(param_hash, ID2SYM(rb_intern("solver_type")), INT2NUM(param->solver_type));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("eps")), DBL2NUM(param->eps));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("C")), DBL2NUM(param->C));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("nr_weight")), INT2NUM(param->nr_weight));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("weight_label")),
               param->weight_label ? convertVectorXiToNArray(param->weight_label, param->nr_weight) : Qnil);
  rb_hash_aset(param_hash, ID2SYM(rb_intern("weight")),
               param->weight ? convertVectorXdToNArray(param->weight, param->nr_weight) : Qnil);
  rb_hash_aset(param_hash, ID2SYM(rb_intern("p")), DBL2NUM(param->p));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("nu")), DBL2NUM(param->nu));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("init_sol")), Qnil);
  rb_hash_aset(param_hash, ID2SYM(rb_intern("w_recalc")), param->w_recalc ? Qtrue : Qfalse);
  return param_hash;
}

LibLinearProblem* convertDatasetToLibLinearProblem(VALUE x_val, VALUE y_val) {
  narray_t* x_nary;
  GetNArray(x_val, x_nary);
  const int n_samples = (int)NA_SHAPE(x_nary)[0];
  const int n_features = (int)NA_SHAPE(x_nary)[1];
  const double* const x_ptr = (double*)na_get_pointer_for_read(x_val);
  const double* const y_ptr = (double*)na_get_pointer_for_read(y_val);

  LibLinearProblem* problem = ALLOC(LibLinearProblem);
  problem->bias = -1;
  problem->n = n_features;
  problem->l = n_samples;
  problem->x = ALLOC_N(LibLinearNode*, n_samples);
  problem->y = ALLOC_N(double, n_samples);

  int last_feature_id = 0;
  bool is_padded = false;
  for (int i = 0; i < n_samples; i++) {
    int n_nonzero_features = 0;
    for (int j = 0; j < n_features; j++) {
      if (x_ptr[i * n_features + j] != 0.0) {
        n_nonzero_features++;
        last_feature_id = j + 1;
      }
    }
    if (!is_padded && last_feature_id == n_features) is_padded = true;
    if (is_padded) {
      problem->x[i] = ALLOC_N(struct feature_node, n_nonzero_features + 1);
    } else {
      problem->x[i] = ALLOC_N(struct feature_node, n_nonzero_features + 2);
    }
    for (int j = 0, k = 0; j < n_features; j++) {
      if (x_ptr[i * n_features + j] != 0.0) {
        problem->x[i][k].index = j + 1;
        problem->x[i][k].value = x_ptr[i * n_features + j];
        k++;
      }
    }
    if (is_padded) {
      problem->x[i][n_nonzero_features].index = -1;
      problem->x[i][n_nonzero_features].value = 0.0;
    } else {
      problem->x[i][n_nonzero_features].index = n_features;
      problem->x[i][n_nonzero_features].value = 0.0;
      problem->x[i][n_nonzero_features + 1].index = -1;
      problem->x[i][n_nonzero_features + 1].value = 0.0;
    }
    problem->y[i] = y_ptr[i];
  }

  RB_GC_GUARD(x_val);
  RB_GC_GUARD(y_val);

  return problem;
}

/** UTILITIES */
bool isSingleOutputModel(LibLinearModel* model) { return (model->nr_class == 2 && model->param.solver_type != MCSVM_CS); }

bool isProbabilisticModel(LibLinearModel* model) {
  return (model->param.solver_type == L2R_LR || model->param.solver_type == L1R_LR || model->param.solver_type == L2R_LR_DUAL);
}

void deleteLibLinearModel(LibLinearModel* model) {
  if (model) {
    xfree(model->w);
    model->w = NULL;
    xfree(model->label);
    model->label = NULL;
    xfree(model);
    model = NULL;
  }
}

void deleteLibLinearParameter(LibLinearParameter* param) {
  if (param) {
    if (param->weight_label) {
      xfree(param->weight_label);
      param->weight_label = NULL;
    }
    if (param->weight) {
      xfree(param->weight);
      param->weight = NULL;
    }
    if (param->init_sol) {
      xfree(param->init_sol);
      param->init_sol = NULL;
    }
    xfree(param);
    param = NULL;
  }
}

void deleteLibLinearProblem(LibLinearProblem* problem) {
  if (problem) {
    if (problem->x) {
      for (int i = 0; i < problem->l; i++) {
        if (problem->x[i]) {
          xfree(problem->x[i]);
          problem->x[i] = NULL;
        }
      }
      xfree(problem->x);
      problem->x = NULL;
    }
    if (problem->y) {
      xfree(problem->y);
      problem->y = NULL;
    }
    xfree(problem);
    problem = NULL;
  }
}

/** MODULE FUNCTIONS */
static VALUE numo_liblinear_train(VALUE self, VALUE x_val, VALUE y_val, VALUE param_hash) {
  if (CLASS_OF(x_val) != numo_cDFloat) x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  if (CLASS_OF(y_val) != numo_cDFloat) y_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, y_val);
  if (!RTEST(nary_check_contiguous(x_val))) x_val = nary_dup(x_val);
  if (!RTEST(nary_check_contiguous(y_val))) y_val = nary_dup(y_val);

  narray_t* x_nary;
  narray_t* y_nary;
  GetNArray(x_val, x_nary);
  GetNArray(y_val, y_nary);
  if (NA_NDIM(x_nary) != 2) {
    rb_raise(rb_eArgError, "Expect samples to be 2-D array.");
    return Qnil;
  }
  if (NA_NDIM(y_nary) != 1) {
    rb_raise(rb_eArgError, "Expect label or target values to be 1-D arrray.");
    return Qnil;
  }
  if (NA_SHAPE(x_nary)[0] != NA_SHAPE(y_nary)[0]) {
    rb_raise(rb_eArgError, "Expect to have the same number of samples for samples and labels.");
    return Qnil;
  }

  VALUE random_seed = rb_hash_aref(param_hash, ID2SYM(rb_intern("random_seed")));
  if (!NIL_P(random_seed)) srand(NUM2UINT(random_seed));

  LibLinearParameter* param = convertHashToLibLinearParameter(param_hash);
  LibLinearProblem* problem = convertDatasetToLibLinearProblem(x_val, y_val);

  const char* err_msg = check_parameter(problem, param);
  if (err_msg) {
    deleteLibLinearProblem(problem);
    deleteLibLinearParameter(param);
    rb_raise(rb_eArgError, "Invalid LIBLINEAR parameter is given: %s", err_msg);
    return Qnil;
  }

  VALUE verbose = rb_hash_aref(param_hash, ID2SYM(rb_intern("verbose")));
  if (!RTEST(verbose)) set_print_string_function(printNull);

  LibLinearModel* model = train(problem, param);
  VALUE model_hash = convertLibLinearModelToHash(model);
  free_and_destroy_model(&model);

  deleteLibLinearProblem(problem);
  deleteLibLinearParameter(param);

  RB_GC_GUARD(x_val);
  RB_GC_GUARD(y_val);

  return model_hash;
}

static VALUE numo_liblinear_cross_validation(VALUE self, VALUE x_val, VALUE y_val, VALUE param_hash, VALUE nr_folds) {
  if (CLASS_OF(x_val) != numo_cDFloat) x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  if (CLASS_OF(y_val) != numo_cDFloat) y_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, y_val);
  if (!RTEST(nary_check_contiguous(x_val))) x_val = nary_dup(x_val);
  if (!RTEST(nary_check_contiguous(y_val))) y_val = nary_dup(y_val);

  narray_t* x_nary;
  narray_t* y_nary;
  GetNArray(x_val, x_nary);
  GetNArray(y_val, y_nary);
  if (NA_NDIM(x_nary) != 2) {
    rb_raise(rb_eArgError, "Expect samples to be 2-D array.");
    return Qnil;
  }
  if (NA_NDIM(y_nary) != 1) {
    rb_raise(rb_eArgError, "Expect label or target values to be 1-D arrray.");
    return Qnil;
  }
  if (NA_SHAPE(x_nary)[0] != NA_SHAPE(y_nary)[0]) {
    rb_raise(rb_eArgError, "Expect to have the same number of samples for samples and labels.");
    return Qnil;
  }

  VALUE random_seed = rb_hash_aref(param_hash, ID2SYM(rb_intern("random_seed")));
  if (!NIL_P(random_seed)) srand(NUM2UINT(random_seed));

  LibLinearParameter* param = convertHashToLibLinearParameter(param_hash);
  LibLinearProblem* problem = convertDatasetToLibLinearProblem(x_val, y_val);

  const char* err_msg = check_parameter(problem, param);
  if (err_msg) {
    deleteLibLinearProblem(problem);
    deleteLibLinearParameter(param);
    rb_raise(rb_eArgError, "Invalid LIBLINEAR parameter is given: %s", err_msg);
    return Qnil;
  }

  size_t t_shape[1] = {(size_t)(problem->l)};
  VALUE t_val = rb_narray_new(numo_cDFloat, 1, t_shape);
  double* t_ptr = (double*)na_get_pointer_for_write(t_val);

  VALUE verbose = rb_hash_aref(param_hash, ID2SYM(rb_intern("verbose")));
  if (!RTEST(verbose)) set_print_string_function(printNull);

  const int n_folds = NUM2INT(nr_folds);
  cross_validation(problem, param, n_folds, t_ptr);

  deleteLibLinearProblem(problem);
  deleteLibLinearParameter(param);

  RB_GC_GUARD(x_val);
  RB_GC_GUARD(y_val);

  return t_val;
}

static VALUE numo_liblinear_predict(VALUE self, VALUE x_val, VALUE param_hash, VALUE model_hash) {
  if (CLASS_OF(x_val) != numo_cDFloat) x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  if (!RTEST(nary_check_contiguous(x_val))) x_val = nary_dup(x_val);

  narray_t* x_nary;
  GetNArray(x_val, x_nary);
  if (NA_NDIM(x_nary) != 2) {
    rb_raise(rb_eArgError, "Expect samples to be 2-D array.");
    return Qnil;
  }

  LibLinearParameter* param = convertHashToLibLinearParameter(param_hash);
  LibLinearModel* model = convertHashToLibLinearModel(model_hash);
  model->param = *param;

  const int n_samples = (int)NA_SHAPE(x_nary)[0];
  const int n_features = (int)NA_SHAPE(x_nary)[1];
  size_t y_shape[1] = {(size_t)n_samples};
  VALUE y_val = rb_narray_new(numo_cDFloat, 1, y_shape);
  double* y_ptr = (double*)na_get_pointer_for_write(y_val);
  const double* const x_ptr = (double*)na_get_pointer_for_read(x_val);
  for (int i = 0; i < n_samples; i++) {
    LibLinearNode* x_nodes = convertVectorXdToLibLinearNode(&x_ptr[i * n_features], n_features);
    y_ptr[i] = predict(model, x_nodes);
    xfree(x_nodes);
  }

  deleteLibLinearModel(model);
  deleteLibLinearParameter(param);

  RB_GC_GUARD(x_val);

  return y_val;
}

static VALUE numo_liblinear_decision_function(VALUE self, VALUE x_val, VALUE param_hash, VALUE model_hash) {
  if (CLASS_OF(x_val) != numo_cDFloat) x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  if (!RTEST(nary_check_contiguous(x_val))) x_val = nary_dup(x_val);

  narray_t* x_nary;
  GetNArray(x_val, x_nary);
  if (NA_NDIM(x_nary) != 2) {
    rb_raise(rb_eArgError, "Expect samples to be 2-D array.");
    return Qnil;
  }

  LibLinearParameter* param = convertHashToLibLinearParameter(param_hash);
  LibLinearModel* model = convertHashToLibLinearModel(model_hash);
  model->param = *param;

  const int n_samples = (int)NA_SHAPE(x_nary)[0];
  const int n_features = (int)NA_SHAPE(x_nary)[1];
  const int y_cols = isSingleOutputModel(model) ? 1 : model->nr_class;
  size_t y_shape[2] = {(size_t)n_samples, (size_t)y_cols};
  const int n_dims = isSingleOutputModel(model) ? 1 : 2;
  VALUE y_val = rb_narray_new(numo_cDFloat, n_dims, y_shape);
  const double* const x_ptr = (double*)na_get_pointer_for_read(x_val);
  double* y_ptr = (double*)na_get_pointer_for_write(y_val);

  for (int i = 0; i < n_samples; i++) {
    LibLinearNode* x_nodes = convertVectorXdToLibLinearNode(&x_ptr[i * n_features], n_features);
    predict_values(model, x_nodes, &y_ptr[i * y_cols]);
    xfree(x_nodes);
  }

  deleteLibLinearModel(model);
  deleteLibLinearParameter(param);

  RB_GC_GUARD(x_val);

  return y_val;
}

static VALUE numo_liblinear_predict_proba(VALUE self, VALUE x_val, VALUE param_hash, VALUE model_hash) {
  narray_t* x_nary;
  GetNArray(x_val, x_nary);
  if (NA_NDIM(x_nary) != 2) {
    rb_raise(rb_eArgError, "Expect samples to be 2-D array.");
    return Qnil;
  }

  LibLinearParameter* param = convertHashToLibLinearParameter(param_hash);
  LibLinearModel* model = convertHashToLibLinearModel(model_hash);
  model->param = *param;

  if (!isProbabilisticModel(model)) {
    deleteLibLinearModel(model);
    deleteLibLinearParameter(param);
    return Qnil;
  }

  if (CLASS_OF(x_val) != numo_cDFloat) x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  if (!RTEST(nary_check_contiguous(x_val))) x_val = nary_dup(x_val);

  const int n_samples = (int)NA_SHAPE(x_nary)[0];
  const int n_features = (int)NA_SHAPE(x_nary)[1];
  size_t y_shape[2] = {(size_t)n_samples, (size_t)(model->nr_class)};
  VALUE y_val = rb_narray_new(numo_cDFloat, 2, y_shape);
  const double* const x_ptr = (double*)na_get_pointer_for_read(x_val);
  double* y_ptr = (double*)na_get_pointer_for_write(y_val);
  for (int i = 0; i < n_samples; i++) {
    LibLinearNode* x_nodes = convertVectorXdToLibLinearNode(&x_ptr[i * n_features], n_features);
    predict_probability(model, x_nodes, &y_ptr[i * model->nr_class]);
    xfree(x_nodes);
  }

  deleteLibLinearModel(model);
  deleteLibLinearParameter(param);

  RB_GC_GUARD(x_val);

  return y_val;
}

static VALUE numo_liblinear_load_model(VALUE self, VALUE filename) {
  const char* const filename_ = StringValuePtr(filename);
  LibLinearModel* model = load_model(filename_);
  if (model == NULL) {
    rb_raise(rb_eIOError, "Failed to load file '%s'", filename_);
    return Qnil;
  }

  VALUE param_hash = convertLibLinearParameterToHash(&(model->param));
  VALUE model_hash = convertLibLinearModelToHash(model);
  free_and_destroy_model(&model);

  VALUE res = rb_ary_new2(2);
  rb_ary_store(res, 0, param_hash);
  rb_ary_store(res, 1, model_hash);

  RB_GC_GUARD(filename);

  return res;
}

static VALUE numo_liblinear_save_model(VALUE self, VALUE filename, VALUE param_hash, VALUE model_hash) {
  LibLinearParameter* param = convertHashToLibLinearParameter(param_hash);
  LibLinearModel* model = convertHashToLibLinearModel(model_hash);
  model->param = *param;

  const char* const filename_ = StringValuePtr(filename);
  const int res = save_model(filename_, model);

  deleteLibLinearModel(model);
  deleteLibLinearParameter(param);

  if (res < 0) {
    rb_raise(rb_eIOError, "Failed to save file '%s'", filename_);
    return Qfalse;
  }

  RB_GC_GUARD(filename);

  return Qtrue;
}

#endif /* LIBLINEAREXT_HPP */
