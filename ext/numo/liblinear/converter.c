#include "converter.h"

VALUE int_vec_to_nary(int* const arr, int const size)
{
  int i;
  size_t shape[1] = { size };
  VALUE v = rb_narray_new(numo_cInt32, 1, shape);
  int32_t* vp = (int32_t*)na_get_pointer_for_write(v);
  for (i = 0; i < size; i++) { vp[i] = (int32_t)arr[i]; }
  return v;
}

int* nary_to_int_vec(VALUE vec_val)
{
  int i;
  int n_elements;
  narray_t* vec_nary;
  int32_t* vec_pt;
  int* vec;

  if (vec_val == Qnil) return NULL;

  GetNArray(vec_val, vec_nary);
  n_elements = (int)NA_SHAPE(vec_nary)[0];

  vec = ALLOC_N(int, n_elements);
  vec_pt = (int32_t*)na_get_pointer_for_read(vec_val);
  for (i = 0; i < n_elements; i++) { vec[i] = (int)vec_pt[i]; }

  return vec;
}

VALUE dbl_vec_to_nary(double* const arr, int const size)
{
  int i;
  size_t shape[1] = { size };
  VALUE v = rb_narray_new(numo_cDFloat, 1, shape);
  double* vp = (double*)na_get_pointer_for_write(v);
  for (i = 0; i < size; i++) { vp[i] = arr[i]; }
  return v;
}

double* nary_to_dbl_vec(VALUE vec_val)
{
  int n_elements;
  narray_t* vec_nary;
  double* vec_pt;
  double* vec;

  if (vec_val == Qnil) return NULL;

  GetNArray(vec_val, vec_nary);
  n_elements = (int)NA_SHAPE(vec_nary)[0];

  vec = ALLOC_N(double, n_elements);
  vec_pt = (double*)na_get_pointer_for_read(vec_val);
  memcpy(vec, vec_pt, n_elements * sizeof(double));

  return vec;
}

VALUE dbl_mat_to_nary(double** const mat, int const n_rows, int const n_cols)
{
  int i, j;
  size_t shape[2] = { n_rows, n_cols };
  VALUE v = rb_narray_new(numo_cDFloat, 2, shape);
  double* vp = (double*)na_get_pointer_for_write(v);

  for (i = 0; i < n_rows; i++) {
    for (j = 0; j < n_cols; j++) {
      vp[i * n_cols + j] = mat[i][j];
    }
  }

  return v;
}

double** nary_to_dbl_mat(VALUE mat_val)
{
  int i, j;
  int n_rows, n_cols;
  narray_t* mat_nary;
  double* mat_pt;
  double** mat;

  if (mat_val == Qnil) return NULL;

  GetNArray(mat_val, mat_nary);
  n_rows = (int)NA_SHAPE(mat_nary)[0];
  n_cols = (int)NA_SHAPE(mat_nary)[1];

  mat_pt = (double*)na_get_pointer_for_read(mat_val);
  mat = ALLOC_N(double*, n_rows);
  for (i = 0; i < n_rows; i++) {
    mat[i] = ALLOC_N(double, n_cols);
    for (j = 0; j < n_cols; j++) {
      mat[i][j] = mat_pt[i * n_cols + j];
    }
  }

  return mat;
}

struct feature_node* dbl_vec_to_node(double* const arr, int const size)
{
  int i, j;
  int n_nonzero_elements;
  struct feature_node* node;

  n_nonzero_elements = 0;
  for (i = 0; i < size; i++) {
    if (arr[i] != 0.0) n_nonzero_elements++;
  }

  node = ALLOC_N(struct feature_node, n_nonzero_elements + 1);
  for (i = 0, j = 0; i < size; i++) {
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
