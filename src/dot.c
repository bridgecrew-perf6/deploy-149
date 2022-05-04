#include "../include/mnblas.h"
#include "../include/complexe2.h"
#include <stdlib.h>
#include <stdio.h>

/*
* 1 FLOP
*/
float mncblas_sdot(const int N, const float *X, const int incX, ////////////////////////////////////
                   const float *Y, const int incY)
{
  register unsigned int i = 0;
  //on part du postulat que incX = incY

  float dot = 0.0;
  #pragma omp parallel for firstprivate(incX) reduction(+:dot)
  for (i = 0; i < N; i += incX)
  {
    dot += X[i] * Y[i];
  }

  return dot;
}

/*
* 1 FLOP
*/
double mncblas_ddot(const int N, const double *X, const int incX,
                    const double *Y, const int incY)
{
  register unsigned int i = 0;
  //on part du postulat que incX = incY
  double dot = 0.0;
  #pragma omp parallel for reduction (+:dot)

  for (i = 0; i < N; i += incX)
  {
    dot += X[i] * Y[i];
  }

  return dot;
}

/*
* 8 FLOP
*/
void mncblas_cdotu_sub(const int N, const void *X, const int incX, /////////////////////
                       const void *Y, const int incY, void *dotu)
{
  register unsigned int i = 0;
  //on part du postulat que incX = incY
  float dot_r = 0.0;
  float dot_i = 0.0;
  register complexe_float_t mult;
  #pragma omp parallel for private(mult) reduction (+:dot_r) reduction (+:dot_i)

  for (i = 0; i < N; i += incX)
  {
    mult = mult_complexe_float(((complexe_float_t *)X)[i], ((complexe_float_t *)Y)[i]); // 6 FLOP
  

    dot_r += mult.real;
    dot_i += mult.imaginary;

    //dot = add_complexe_float(dot, mult);  // 2 FLOP
  }
  ((complexe_float_t *)dotu)->real = dot_r;
  ((complexe_float_t *)dotu)->imaginary = dot_i;
/*
  to_ret->real = dot_r;
  to_ret->imaginary = dot_i;
*/

}

/*
* 9 FLOP
*/
void mncblas_cdotc_sub(const int N, const void *X, const int incX,
                       const void *Y, const int incY, void *dotc)
{
  register unsigned int i = 0;
  //on part du postulat que incX = incY

  float dot_r = 0.0;
  float dot_i = 0.0;

  //complexe_float_t dot;
  complexe_float_t mult, conjuge_X;
  #pragma omp parallel for private(mult, conjuge_X) reduction(+:dot_r) reduction(+:dot_i)

  for (i = 0; i < N; i += incX)
  {
    conjuge_X.real = ((complexe_float_t *)X)[i].real;
    conjuge_X.imaginary = -((complexe_float_t *)X)[i].imaginary;
    mult = mult_complexe_float(conjuge_X, ((complexe_float_t *)Y)[i]);

    dot_r += mult.real;
    dot_i += mult.imaginary;

    //dot = add_complexe_float(dot, mult);
  }
  //*(complexe_float_t *)dotc = dot;
  ((complexe_float_t *)dotc)->real = dot_r;
  ((complexe_float_t *)dotc)->imaginary = dot_i;

  return;
}

/*
* 8 FLOP
*/
void mncblas_zdotu_sub(const int N, const void *X, const int incX,
                       const void *Y, const int incY, void *dotu)
{
  register unsigned int i = 0;
  //on part du postulat que incX = incY
  double dot_r = 0.0;
  double dot_i = 0.0;
    
  complexe_double_t mult;

  #pragma omp parallel for private(mult) reduction(+:dot_r) reduction(+:dot_i)

  for (i = 0; i < N; i += incX)
  {
    mult = mult_complexe_double(((complexe_double_t *)X)[i], ((complexe_double_t *)Y)[i]);

    dot_r += mult.real;
    dot_i += mult.imaginary;

    //dot = add_complexe_double(dot, mult);
  }
  //*(complexe_double_t *)dotu = dot;
  ((complexe_double_t *)dotu)->real = dot_r;
  ((complexe_double_t *)dotu)->imaginary = dot_i;

}

/*
* 9 FLOP
*/
void mncblas_zdotc_sub(const int N, const void *X, const int incX,
                       const void *Y, const int incY, void *dotc)
{
  register unsigned int i = 0;
  //on part du postulat que incX = incY
  //register complexe_double_t dot;
  double dot_r = 0.0;
  double dot_i = 0.0;
  register complexe_double_t mult, conjuge_X;
  #pragma omp parallel for private(mult, conjuge_X) reduction(+:dot_r) reduction(+:dot_i)

  for (i = 0; i < N; i += incX)
  {
    conjuge_X.real = ((complexe_double_t *)X)[i].real;
    conjuge_X.imaginary = -((complexe_double_t *)X)[i].imaginary;
    mult = mult_complexe_double(conjuge_X, ((complexe_double_t *)Y)[i]);
    dot_r += mult.real;
    dot_i += mult.imaginary;

    //dot = add_complexe_double(dot, mult);
  }
  //*(complexe_double_t *)dotc = dot;
  ((complexe_double_t *)dotc)->real = dot_r;
  ((complexe_double_t *)dotc)->imaginary = dot_i;

  return;
}
