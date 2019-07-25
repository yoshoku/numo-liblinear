#include "model.h"

struct model* rb_hash_to_model(VALUE model_hash)
{
  VALUE el;
  struct model* model = ALLOC(struct model);
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("nr_class")));
  model->nr_class = el != Qnil ? NUM2INT(el) : 0;
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("nr_feature")));
  model->nr_feature = el != Qnil ? NUM2INT(el) : 0;
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("w")));
  model->w = nary_to_dbl_vec(el);
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("label")));
  model->label = nary_to_int_vec(el);
  el = rb_hash_aref(model_hash, ID2SYM(rb_intern("bias")));
  model->bias = NUM2DBL(el);
  return model;
}

VALUE model_to_rb_hash(struct model* const model)
{
  int const n_cols = model->nr_class > 2 ? model->nr_class : 1;
  int const n_rows = model->nr_feature;
  VALUE model_hash = rb_hash_new();
  rb_hash_aset(model_hash, ID2SYM(rb_intern("nr_class")), INT2NUM(model->nr_class));
  rb_hash_aset(model_hash, ID2SYM(rb_intern("nr_feature")), INT2NUM(model->nr_feature));
  rb_hash_aset(model_hash, ID2SYM(rb_intern("w")),
      model->w ? dbl_vec_to_nary(model->w, n_rows * n_cols) : Qnil);
  rb_hash_aset(model_hash, ID2SYM(rb_intern("label")),
      model->label ? int_vec_to_nary(model->label, model->nr_class) : Qnil);
  rb_hash_aset(model_hash, ID2SYM(rb_intern("bias")), DBL2NUM(model->bias));
  return model_hash;
}

void xfree_model(struct model* model)
{
  if (model) {
    xfree(model->w);
    model->w = NULL;
    xfree(model->label);
    model->label = NULL;
    xfree(model);
    model = NULL;
  }
}
