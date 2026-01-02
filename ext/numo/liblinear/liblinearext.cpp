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

#include "liblinearext.hpp"

extern "C" void Init_liblinearext(void) {
  rb_require("numo/narray");

  /**
   * Document-module: Numo
   * Numo is the top level namespace of NUmerical MOdules for Ruby.
   */

  /**
   * Document-module: Numo::Liblinear
   * Numo::Liblinear is a binding library for LIBLINEAR that handles dataset with Numo::NArray.
   */
  VALUE mLiblinear = rb_define_module_under(mNumo, "Liblinear");

  /* The version of LIBLINEAR used in backgroud library. */
  rb_define_const(mLiblinear, "LIBLINEAR_VERSION", INT2NUM(LIBLINEAR_VERSION));

  /**
   * Document-module: Numo::Liblinear::SolverType
   * The module consisting of constants for solver type that used for parameter of LIBLINER.
   */
  VALUE mSolverType = rb_define_module_under(mLiblinear, "SolverType");
  /* L2-regularized logistic regression (primal) */
  rb_define_const(mSolverType, "L2R_LR", INT2NUM(L2R_LR));
  /* L2-regularized L2-loss support vector classification (dual) */
  rb_define_const(mSolverType, "L2R_L2LOSS_SVC_DUAL", INT2NUM(L2R_L2LOSS_SVC_DUAL));
  /* L2-regularized L2-loss support vector classification (primal) */
  rb_define_const(mSolverType, "L2R_L2LOSS_SVC", INT2NUM(L2R_L2LOSS_SVC));
  /* L2-regularized L1-loss support vector classification (dual) */
  rb_define_const(mSolverType, "L2R_L1LOSS_SVC_DUAL", INT2NUM(L2R_L1LOSS_SVC_DUAL));
  /* support vector classification by Crammer and Singer */
  rb_define_const(mSolverType, "MCSVM_CS", INT2NUM(MCSVM_CS));
  /* L1-regularized L2-loss support vector classification */
  rb_define_const(mSolverType, "L1R_L2LOSS_SVC", INT2NUM(L1R_L2LOSS_SVC));
  /* L1-regularized logistic regression */
  rb_define_const(mSolverType, "L1R_LR", INT2NUM(L1R_LR));
  /* L2-regularized logistic regression (dual) */
  rb_define_const(mSolverType, "L2R_LR_DUAL", INT2NUM(L2R_LR_DUAL));
  /* L2-regularized L2-loss support vector regression (primal) */
  rb_define_const(mSolverType, "L2R_L2LOSS_SVR", INT2NUM(L2R_L2LOSS_SVR));
  /* L2-regularized L2-loss support vector regression (dual) */
  rb_define_const(mSolverType, "L2R_L2LOSS_SVR_DUAL", INT2NUM(L2R_L2LOSS_SVR_DUAL));
  /* L2-regularized L1-loss support vector regression (dual) */
  rb_define_const(mSolverType, "L2R_L1LOSS_SVR_DUAL", INT2NUM(L2R_L1LOSS_SVR_DUAL));
  /* one-class support vector machine (dual) */
  rb_define_const(mSolverType, "ONECLASS_SVM", INT2NUM(ONECLASS_SVM));

  /**
   * Train the model according to the given training data.
   *
   * @overload train(x, y, param) -> Hash
   *   @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to be used for training the model.
   *   @param y [Numo::DFloat] (shape: [n_samples]) The labels or target values for samples.
   *   @param param [Hash] The parameters of a model.
   *
   * @example
   *   require 'numo/liblinear'
   *
   *   # Prepare training dataset.
   *   x = Numo::DFloat[[-0.8, 1.0], [-0.5, 0.8], [0.9, -0.8], [0.8, -0.7]]
   *   y = Numo::Int32[-1, -1, 1, 1]
   *
   *   # Train L2-regularized L2-loss support vector classifier.
   *   param = {
   *     solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL,
   *     C: 0.1,
   *     random_seed: 1
   *   }
   *   model = Numo::Liblinear.train(x, y, param)
   *
   *   # Predict labels of test data.
   *   x_test = Numo::DFloat[[-0.7, 0.9], [0.5, -0.4]]
   *   result = Numo::Liblinear.predict(x_test, param, model)
   *   p result
   *   # Numo::DFloat#shape=[2]
   *   # [-1, 1]
   *
   * @raise [ArgumentError] If the sample array is not 2-dimensional, the label array is not 1-dimensional,
   *   the sample array and label array do not have the same number of samples, or
   *   the hyperparameter has an invalid value, this error is raised.
   * @return [Hash] The model obtained from the training procedure.
   */
  rb_define_module_function(mLiblinear, "train", RUBY_METHOD_FUNC(numo_liblinear_train), 3);
  /**
   * Perform cross validation under given parameters. The given samples are separated to n_fols folds.
   * The predicted labels or values in the validation process are returned.
   *
   * @overload cv(x, y, param, n_folds) -> Numo::DFloat
   *   @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to be used for training the model.
   *   @param y [Numo::DFloat] (shape: [n_samples]) The labels or target values for samples.
   *   @param param [Hash] The parameters of a model.
   *   @param n_folds [Integer] The number of folds.
   *
   * @example
   *   require 'numo/liblinear'
   *
   *   # x: samples
   *   # y: labels
   *
   *   # Define parameters of L2-regularized L2-loss support vector classification.
   *   param = {
   *     solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL,
   *     C: 1,
   *     random_seed: 1,
   *     verbose: true
   *   }
   *
   *   # Perform 5-cross validation.
   *   n_folds = 5
   *   res = Numo::Liblinear::cv(x, y, param, n_folds)
   *
   *   # Print mean accuracy.
   *   mean_accuracy = y.eq(res).count.fdiv(y.size)
   *   puts "Accuracy: %.1f %%" % (100 * mean_accuracy)
   *
   * @raise [ArgumentError] If the sample array is not 2-dimensional, the label array is not 1-dimensional,
   *   the sample array and label array do not have the same number of samples, or
   *   the hyperparameter has an invalid value, this error is raised.
   * @return [Numo::DFloat] (shape: [n_samples]) The predicted class label or value of each sample.
   */
  rb_define_module_function(mLiblinear, "cv", RUBY_METHOD_FUNC(numo_liblinear_cross_validation), 4);
  /**
   * Predict class labels or values for given samples.
   *
   * @overload predict(x, param, model) -> Numo::DFloat
   *   @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to calculate the scores.
   *   @param param [Hash] The parameters of the trained model.
   *   @param model [Hash] The model obtained from the training procedure.
   *
   * @raise [ArgumentError] If the sample array is not 2-dimensional, this error is raised.
   * @return [Numo::DFloat] (shape: [n_samples]) The predicted class label or value of each sample.
   */
  rb_define_module_function(mLiblinear, "predict", RUBY_METHOD_FUNC(numo_liblinear_predict), 3);
  /**
   * Calculate decision values for given samples.
   *
   * @overload decision_function(x, param, model) -> Numo::DFloat
   *   @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to calculate the scores.
   *   @param param [Hash] The parameters of the trained model.
   *   @param model [Hash] The model obtained from the training procedure.
   *
   * @raise [ArgumentError] If the sample array is not 2-dimensional, this error is raised.
   * @return [Numo::DFloat] (shape: [n_samples, n_classes]) The decision value of each sample.
   */
  rb_define_module_function(mLiblinear, "decision_function", RUBY_METHOD_FUNC(numo_liblinear_decision_function), 3);
  /**
   * Predict class probability for given samples.
   * The model must have probability information calcualted in training procedure.
   * The method supports only the logistic regression.
   *
   * @overload predict_proba(x, param, model) -> Numo::DFloat
   *   @param x [Numo::DFloat] (shape: [n_samples, n_features]) The samples to predict the class probabilities.
   *   @param param [Hash] The parameters of the trained Logistic Regression model.
   *   @param model [Hash] The model obtained from the training procedure.
   *
   * @raise [ArgumentError] If the sample array is not 2-dimensional, this error is raised.
   * @return [Numo::DFloat] (shape: [n_samples, n_classes]) Predicted probablity of each class per sample.
   */
  rb_define_module_function(mLiblinear, "predict_proba", RUBY_METHOD_FUNC(numo_liblinear_predict_proba), 3);
  /**
   * Load the parameters and model from a text file with LIBLINEAR format.
   *
   * @overload load_model(filename) -> Array
   *   @param filename [String] The path to a file to load.
   *
   * @raise [IOError] This error raises when failed to load the model file.
   * @return [Array] Array contains the parameters and model.
   */
  rb_define_module_function(mLiblinear, "load_model", RUBY_METHOD_FUNC(numo_liblinear_load_model), 1);
  /**
   * Save the parameters and model as a text file with LIBLINEAR format. The saved file can be used with the liblinear tools.
   * Note that the save_model saves only the parameters necessary for estimation with the trained model.
   *
   * @overload save_model(filename, param, model) -> Boolean
   *   @param filename [String] The path to a file to save.
   *   @param param [Hash] The parameters of the trained model.
   *   @param model [Hash] The model obtained from the training procedure.
   *
   * @raise [IOError] This error raises when failed to save the model file.
   * @return [Boolean] true on success, or false if an error occurs.
   */
  rb_define_module_function(mLiblinear, "save_model", RUBY_METHOD_FUNC(numo_liblinear_save_model), 3);
}
