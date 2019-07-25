#include "solver_type.h"

RUBY_EXTERN VALUE mLiblinear;

void rb_init_solver_type_module()
{
  /**
   * Document-module: Numo::Liblinear::SolverType
   * The module consisting of constants for solver type that used for parameter of LIBLINER.
   */
  VALUE mSolverType = rb_define_module_under(mLiblinear, "SolverType");
  /* L2-regularized logistic regression (primal) */
  rb_define_const(mSolverType, "L2R_LR", INT2NUM(L2R_LR));
  /* L2-regularized L2-loss support vector classification (dual) */
  rb_define_const(mSolverType, "L2R_L2LOSS_SVC_DUAL", INT2NUM(L2R_L2LOSS_SVC_DUAL));
  /* L2-regularized L1-loss support vector classification (dual) */
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
}
