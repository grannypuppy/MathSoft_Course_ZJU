#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_complex.h>
#include <stdio.h>

int main() {
  // 创建复数向量
  gsl_vector_complex *v_complex = gsl_vector_complex_alloc(3);

  // 创建实数向量
  gsl_vector *v_real = gsl_vector_alloc(3);

  // 设置复数向量的值
  gsl_complex z0 = gsl_complex_rect(4.0, 2.0);
  gsl_complex z1 = gsl_complex_rect(6.0, 3.0);
  gsl_complex z2 = gsl_complex_rect(8.0, 4.0);

  gsl_vector_complex_set(v_complex, 0, z0);
  gsl_vector_complex_set(v_complex, 1, z1);
  gsl_vector_complex_set(v_complex, 2, z2);

  // 设置实数向量的值
  gsl_vector_set(v_real, 0, 2.0);
  gsl_vector_set(v_real, 1, 3.0);
  gsl_vector_set(v_real, 2, 4.0);

  // 输出原始复数向量
  printf("原始复数向量:\n");
  for (size_t i = 0; i < 3; i++) {
    gsl_complex z = gsl_vector_complex_get(v_complex, i);
    printf("(%g, %g)\n", GSL_REAL(z), GSL_IMAG(z));
  }

  // 将复数向量除以实数向量
  gsl_vector_complex_div_real(v_complex, v_real);

  // 输出结果
  printf("\n除以实数向量后的复数向量:\n");
  for (size_t i = 0; i < 3; i++) {
    gsl_complex z = gsl_vector_complex_get(v_complex, i);
    printf("(%g, %g)\n", GSL_REAL(z), GSL_IMAG(z));
  }

  // 释放内存
  gsl_vector_complex_free(v_complex);
  gsl_vector_free(v_real);

  return 0;
}
