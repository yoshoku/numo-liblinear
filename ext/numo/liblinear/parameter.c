#include "parameter.h"

struct parameter* rb_hash_to_parameter(VALUE param_hash)
{
  VALUE el;
  struct parameter* param = ALLOC(struct parameter);
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("solver_type")));
  param->solver_type = !NIL_P(el) ? NUM2INT(el) : L2R_L2LOSS_SVC_DUAL;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("eps")));
  if (!NIL_P(el)) {
    param->eps = NUM2DBL(el);
  } else {
    switch(param->solver_type)
    {
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
    memcpy(param->weight_label, (int32_t*)na_get_pointer_for_read(el), param->nr_weight);
  }
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("weight")));
  param->weight = NULL;
  if (!NIL_P(el)) {
    param->weight = ALLOC_N(double, param->nr_weight);
    memcpy(param->weight, (double*)na_get_pointer_for_read(el), param->nr_weight);
  }
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("p")));
  param->p = !NIL_P(el) ? NUM2DBL(el) : 0.1;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("nu")));
  param->nu = !NIL_P(el) ? NUM2DBL(el) : 0.5;
  el = rb_hash_aref(param_hash, ID2SYM(rb_intern("init_sol")));
  param->init_sol = NULL;
  if (!NIL_P(el)) {
    param->init_sol = nary_to_dbl_vec(el);
  }
  param->regularize_bias = 1;
  return param;
}

VALUE parameter_to_rb_hash(struct parameter* const param)
{
  VALUE param_hash = rb_hash_new();
  rb_hash_aset(param_hash, ID2SYM(rb_intern("solver_type")), INT2NUM(param->solver_type));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("eps")), DBL2NUM(param->eps));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("C")), DBL2NUM(param->C));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("nr_weight")), INT2NUM(param->nr_weight));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("weight_label")),
    param->weight_label ? int_vec_to_nary(param->weight_label, param->nr_weight) : Qnil);
  rb_hash_aset(param_hash, ID2SYM(rb_intern("weight")),
    param->weight ? dbl_vec_to_nary(param->weight, param->nr_weight) : Qnil);
  rb_hash_aset(param_hash, ID2SYM(rb_intern("p")), DBL2NUM(param->p));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("nu")), DBL2NUM(param->nu));
  rb_hash_aset(param_hash, ID2SYM(rb_intern("init_sol")), Qnil);
  return param_hash;
}

void xfree_parameter(struct parameter* param)
{
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
