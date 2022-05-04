#include <stdio.h>
#include <x86intrin.h>

#include "../include/mnblas.h"
#include "../include/complexe.h"

#include "flop.h"

#define VECSIZE 500000

#define NB_FOIS 41943

typedef float vfloat[VECSIZE];
typedef complexe_float_t vComplexefloat[VECSIZE];

vfloat vec1, vec2;
vComplexefloat vecComplexef1, vecComplexef2;

void vector_init(vfloat V, float x)
{
  register unsigned int i;

  for (i = 0; i < VECSIZE; i++)
    V[i] = x;

  return;
}

void vector_init_complexe(vComplexefloat V, float reel, float imaginaire)
{
  register unsigned int i;

  for (i = 0; i < VECSIZE; i++)
  {
    V[i].real = reel;
    V[i].imaginary = imaginaire;
  }

  return;
}

void vector_print(vfloat V)
{
  register unsigned int i;

  for (i = 0; i < VECSIZE; i++)
    printf("%f ", V[i]);
  printf("\n");

  return;
}

int main(int argc, char **argv)
{
  unsigned long long start, end;
  float res;
  int i;

  double somme_double = 0.f;
  double somme_double_cp = 0.f;

  init_flop();
  for (i = 0; i < NB_FOIS; i++)
  {
    vector_init(vec1, 1.0);
    vector_init(vec2, 2.0);
    res = 0.0;

    start = _rdtsc();
    res = mncblas_sdot(VECSIZE, vec1, 1, vec2, 1);
    end = _rdtsc();

    //printf("mncblas_sdot %d : res = %3.2f nombre de cycles: %Ld \n", i, res, end - start);
    somme_double += calcul_flop_ret("sdot ", 2 * VECSIZE, end - start);
  }

  printf("\n");
  init_flop();
  complexe_float_t resComplexe ;
  for (i = 0; i < NB_FOIS; i++)
  {
    vector_init_complexe(vecComplexef1, 1.0, 0.0);
    vector_init_complexe(vecComplexef2, 2.0, 0.0);
    resComplexe.real = 0.0;
    resComplexe.imaginary = 0.0;

    start = _rdtsc();
    mncblas_cdotu_sub(VECSIZE, vecComplexef1, 1, vecComplexef2, 1, &resComplexe);
    end = _rdtsc();


    //printf("mncblas_cdotu_sub %d : res = %3.2f +i %3.2f nombre de cycles: %Ld \n", i, resComplexe.real, resComplexe.imaginary, end - start);

    somme_double_cp += calcul_flop_ret("sdot ", 8 * VECSIZE, end - start);
  }

   printf("\n");

    printf("\n=========== Résultats DOT double précision ===========\n");
    printf("En moyenne on a une performance de %5.3f GFlop/s\n", somme_double/NB_FOIS);
    printf("\n=========== Résultats DOT double précision complexe ===========\n");
    printf("En moyenne on a une performance de %5.3f GFlop/s\n", somme_double_cp/NB_FOIS);
    return 0;
}
