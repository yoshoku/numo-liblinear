/**
 * LIBLINEAR interface for Numo::NArray
 */
#include "liblinearext.h"

VALUE mNumo;
VALUE mLiblinear;

void print_null(const char *s) {}

/**
 * Train the model according to the given training data.
 *
 * @overload train(x, y, param) -> Hash
 *
 * @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to be used for training the model.
 * @param y [Numo::DFloat] (shape: [n_samples]) The labels or target values for samples.
 * @param param [Hash] The parameters of a model.
 * @return [Hash] The model obtained from the training procedure.
 */
static
VALUE numo_liblinear_train(VALUE self, VALUE x_val, VALUE y_val, VALUE param_hash)
{
  struct problem* problem;
  struct parameter* param;
  struct model* model;
  VALUE model_hash;

  if (CLASS_OF(x_val) != numo_cDFloat) {
    x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  }
  if (CLASS_OF(y_val) != numo_cDFloat) {
    y_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, y_val);
  }
  if (!RTEST(nary_check_contiguous(x_val))) {
    x_val = nary_dup(x_val);
  }
  if (!RTEST(nary_check_contiguous(y_val))) {
    y_val = nary_dup(y_val);
  }

  param = rb_hash_to_parameter(param_hash);
  problem = dataset_to_problem(x_val, y_val);

  set_print_string_function(print_null);
  model = train(problem, param);
  model_hash = model_to_rb_hash(model);
  free_and_destroy_model(&model);

  xfree_problem(problem);
  xfree_parameter(param);

  return model_hash;
}

/**
 * Perform cross validation under given parameters. The given samples are separated to n_fols folds.
 * The predicted labels or values in the validation process are returned.
 *
 * @overload cv(x, y, param, n_folds) -> Numo::DFloat
 *
 * @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to be used for training the model.
 * @param y [Numo::DFloat] (shape: [n_samples]) The labels or target values for samples.
 * @param param [Hash] The parameters of a model.
 * @param n_folds [Integer] The number of folds.
 * @return [Numo::DFloat] (shape: [n_samples]) The predicted class label or value of each sample.
 */
static
VALUE numo_liblinear_cross_validation(VALUE self, VALUE x_val, VALUE y_val, VALUE param_hash, VALUE nr_folds)
{
  const int n_folds = NUM2INT(nr_folds);
  size_t t_shape[1];
  VALUE t_val;
  double* t_pt;
  struct problem* problem;
  struct parameter* param;

  if (CLASS_OF(x_val) != numo_cDFloat) {
    x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  }
  if (CLASS_OF(y_val) != numo_cDFloat) {
    y_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, y_val);
  }
  if (!RTEST(nary_check_contiguous(x_val))) {
    x_val = nary_dup(x_val);
  }
  if (!RTEST(nary_check_contiguous(y_val))) {
    y_val = nary_dup(y_val);
  }

  param = rb_hash_to_parameter(param_hash);
  problem = dataset_to_problem(x_val, y_val);

  t_shape[0] = problem->l;
  t_val = rb_narray_new(numo_cDFloat, 1, t_shape);
  t_pt = (double*)na_get_pointer_for_write(t_val);

  set_print_string_function(print_null);
  cross_validation(problem, param, n_folds, t_pt);

  xfree_problem(problem);
  xfree_parameter(param);

  return t_val;
}


/**
 * Predict class labels or values for given samples.
 *
 * @overload predict(x, param, model) -> Numo::DFloat
 *
 * @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to calculate the scores.
 * @param param [Hash] The parameters of the trained model.
 * @param model [Hash] The model obtained from the training procedure.
 * @return [Numo::DFloat] (shape: [n_samples]) The predicted class label or value of each sample.
 */
static
VALUE numo_liblinear_predict(VALUE self, VALUE x_val, VALUE param_hash, VALUE model_hash)
{
  struct parameter* param;
  struct model* model;
  struct feature_node* x_nodes;
  narray_t* x_nary;
  double* x_pt;
  size_t y_shape[1];
  VALUE y_val;
  double* y_pt;
  int i, j;
  int n_samples;
  int n_features;

  /* Obtain C data structures. */
  if (CLASS_OF(x_val) != numo_cDFloat) {
    x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  }
  if (!RTEST(nary_check_contiguous(x_val))) {
    x_val = nary_dup(x_val);
  }
  GetNArray(x_val, x_nary);
  param = rb_hash_to_parameter(param_hash);
  model = rb_hash_to_model(model_hash);
  model->param = *param;

  /* Initialize some variables. */
  n_samples = (int)NA_SHAPE(x_nary)[0];
  n_features = (int)NA_SHAPE(x_nary)[1];
  y_shape[0] = n_samples;
  y_val = rb_narray_new(numo_cDFloat, 1, y_shape);
  y_pt = (double*)na_get_pointer_for_write(y_val);
  x_pt = (double*)na_get_pointer_for_read(x_val);

  /* Predict values. */
  x_nodes = ALLOC_N(struct feature_node, n_features + 1);
  x_nodes[n_features].index = -1;
  x_nodes[n_features].value = 0.0;
  for (i = 0; i < n_samples; i++) {
    for (j = 0; j < n_features; j++) {
      x_nodes[j].index = j + 1;
      x_nodes[j].value = (double)x_pt[i * n_features + j];
    }
    y_pt[i] = predict(model, x_nodes);
  }

  xfree(x_nodes);
  xfree_model(model);
  xfree_parameter(param);

  return y_val;
}

/**
 * Calculate decision values for given samples.
 *
 * @overload decision_function(x, param, model) -> Numo::DFloat
 *
 * @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to calculate the scores.
 * @param param [Hash] The parameters of the trained model.
 * @param model [Hash] The model obtained from the training procedure.
 * @return [Numo::DFloat] (shape: [n_samples, n_classes]) The decision value of each sample.
 */
static
VALUE numo_liblinear_decision_function(VALUE self, VALUE x_val, VALUE param_hash, VALUE model_hash)
{
  struct parameter* param;
  struct model* model;
  struct feature_node* x_nodes;
  narray_t* x_nary;
  double* x_pt;
  size_t y_shape[2];
  VALUE y_val;
  double* y_pt;
  double* dec_values;
  int y_cols;
  int i, j;
  int n_samples;
  int n_features;

  /* Obtain C data structures. */
  if (CLASS_OF(x_val) != numo_cDFloat) {
    x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
  }
  if (!RTEST(nary_check_contiguous(x_val))) {
    x_val = nary_dup(x_val);
  }
  GetNArray(x_val, x_nary);
  param = rb_hash_to_parameter(param_hash);
  model = rb_hash_to_model(model_hash);
  model->param = *param;

  /* Initialize some variables. */
  n_samples = (int)NA_SHAPE(x_nary)[0];
  n_features = (int)NA_SHAPE(x_nary)[1];

  if (model->nr_class == 2 && model->param.solver_type != MCSVM_CS) {
    y_shape[0] = n_samples;
    y_shape[1] = 1;
    y_val = rb_narray_new(numo_cDFloat, 1, y_shape);
  } else {
    y_shape[0] = n_samples;
    y_shape[1] = model->nr_class;
    y_val = rb_narray_new(numo_cDFloat, 2, y_shape);
  }

  x_pt = (double*)na_get_pointer_for_read(x_val);
  y_pt = (double*)na_get_pointer_for_write(y_val);

  /* Predict values. */
  if (model->nr_class == 2 && model->param.solver_type != MCSVM_CS) {
    x_nodes = ALLOC_N(struct feature_node, n_features + 1);
    x_nodes[n_features].index = -1;
    x_nodes[n_features].value = 0.0;
    for (i = 0; i < n_samples; i++) {
      for (j = 0; j < n_features; j++) {
        x_nodes[j].index = j + 1;
        x_nodes[j].value = (double)x_pt[i * n_features + j];
      }
      predict_values(model, x_nodes, &y_pt[i]);
    }
    xfree(x_nodes);
  } else {
    y_cols = (int)y_shape[1];
    dec_values = ALLOC_N(double, y_cols);
    x_nodes = ALLOC_N(struct feature_node, n_features + 1);
    x_nodes[n_features].index = -1;
    x_nodes[n_features].value = 0.0;
    for (i = 0; i < n_samples; i++) {
      for (j = 0; j < n_features; j++) {
        x_nodes[j].index = j + 1;
        x_nodes[j].value = (double)x_pt[i * n_features + j];
      }
      predict_values(model, x_nodes, dec_values);
      for (j = 0; j < y_cols; j++) {
        y_pt[i * y_cols + j] = dec_values[j];
      }
    }
    xfree(x_nodes);
    xfree(dec_values);
  }

  xfree_model(model);
  xfree_parameter(param);

  return y_val;
}

/**
 * Predict class probability for given samples.
 * The model must have probability information calcualted in training procedure.
 * The method supports only the logistic regression.
 *
 * @overload predict_proba(x, param, model) -> Numo::DFloat
 *
 * @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to predict the class probabilities.
 * @param param [Hash] The parameters of the trained Logistic Regression model.
 * @param model [Hash] The model obtained from the training procedure.
 * @return [Numo::DFloat] (shape: [n_samples, n_classes]) Predicted probablity of each class per sample.
 */
static
VALUE numo_liblinear_predict_proba(VALUE self, VALUE x_val, VALUE param_hash, VALUE model_hash)
{
  struct parameter* param;
  struct model* model;
  struct feature_node* x_nodes;
  narray_t* x_nary;
  double* x_pt;
  size_t y_shape[2];
  VALUE y_val = Qnil;
  double* y_pt;
  double* probs;
  int i, j;
  int n_samples;
  int n_features;

  param = rb_hash_to_parameter(param_hash);
  model = rb_hash_to_model(model_hash);
  model->param = *param;

  if (model->param.solver_type == L2R_LR || model->param.solver_type == L1R_LR || model->param.solver_type == L2R_LR_DUAL) {
    /* Obtain C data structures. */
    if (CLASS_OF(x_val) != numo_cDFloat) {
      x_val = rb_funcall(numo_cDFloat, rb_intern("cast"), 1, x_val);
    }
    if (!RTEST(nary_check_contiguous(x_val))) {
      x_val = nary_dup(x_val);
    }
    GetNArray(x_val, x_nary);

    /* Initialize some variables. */
    n_samples = (int)NA_SHAPE(x_nary)[0];
    n_features = (int)NA_SHAPE(x_nary)[1];
    y_shape[0] = n_samples;
    y_shape[1] = model->nr_class;
    y_val = rb_narray_new(numo_cDFloat, 2, y_shape);
    x_pt = (double*)na_get_pointer_for_read(x_val);
    y_pt = (double*)na_get_pointer_for_write(y_val);

    /* Predict values. */
    probs = ALLOC_N(double, model->nr_class);
    x_nodes = ALLOC_N(struct feature_node, n_features + 1);
    x_nodes[n_features].index = -1;
    x_nodes[n_features].value = 0.0;
    for (i = 0; i < n_samples; i++) {
      for (j = 0; j < n_features; j++) {
        x_nodes[j].index = j + 1;
        x_nodes[j].value = (double)x_pt[i * n_features + j];
      }
      predict_probability(model, x_nodes, probs);
      for (j = 0; j < model->nr_class; j++) {
        y_pt[i * model->nr_class + j] = probs[j];
      }
    }
    xfree(x_nodes);
    xfree(probs);
  }

  xfree_model(model);
  xfree_parameter(param);

  return y_val;
}

/**
 * Load the parameters and model from a text file with LIBLINEAR format.
 *
 * @param filename [String] The path to a file to load.
 * @return [Array] Array contains the parameters and model.
 */
static
VALUE numo_liblinear_load_model(VALUE self, VALUE filename)
{
  struct model* model = load_model(StringValuePtr(filename));
  VALUE res = rb_ary_new2(2);
  VALUE param_hash = Qnil;
  VALUE model_hash = Qnil;

  if (model) {
    param_hash = parameter_to_rb_hash(&(model->param));
    model_hash = model_to_rb_hash(model);
    free_and_destroy_model(&model);
  }

  rb_ary_store(res, 0, param_hash);
  rb_ary_store(res, 1, model_hash);

  return res;
}

/**
 * Save the parameters and model as a text file with LIBLINEAR format. The saved file can be used with the liblinear tools.
 * Note that the save_model saves only the parameters necessary for estimation with the trained model.
 *
 * @overload save_model(filename, param, model) -> Boolean
 *
 * @param filename [String] The path to a file to save.
 * @param param [Hash] The parameters of the trained model.
 * @param model [Hash] The model obtained from the training procedure.
 * @return [Boolean] true on success, or false if an error occurs.
 */
static
VALUE numo_liblinear_save_model(VALUE self, VALUE filename, VALUE param_hash, VALUE model_hash)
{
  struct parameter* param = rb_hash_to_parameter(param_hash);
  struct model* model = rb_hash_to_model(model_hash);
  int res;

  model->param = *param;
  res = save_model(StringValuePtr(filename), model);

  xfree_model(model);
  xfree_parameter(param);

  return res < 0 ? Qfalse : Qtrue;
}

void Init_liblinearext()
{
  rb_require("numo/narray");

  /**
   * Document-module: Numo
   * Numo is the top level namespace of NUmerical MOdules for Ruby.
   */
  mNumo = rb_define_module("Numo");

  /**
   * Document-module: Numo::Liblinear
   * Numo::Liblinear is a binding library for LIBLINEAR that handles dataset with Numo::NArray.
   */
  mLiblinear = rb_define_module_under(mNumo, "Liblinear");

  rb_define_module_function(mLiblinear, "train", numo_liblinear_train, 3);
  rb_define_module_function(mLiblinear, "cv", numo_liblinear_cross_validation, 4);
  rb_define_module_function(mLiblinear, "predict", numo_liblinear_predict, 3);
  rb_define_module_function(mLiblinear, "decision_function", numo_liblinear_decision_function, 3);
  rb_define_module_function(mLiblinear, "predict_proba", numo_liblinear_predict_proba, 3);
  rb_define_module_function(mLiblinear, "load_model", numo_liblinear_load_model, 1);
  rb_define_module_function(mLiblinear, "save_model", numo_liblinear_save_model, 3);

  rb_init_solver_type_module();
}
