#include <stdio.h>
#include <x86intrin.h>

// La frequence du processeur est de 2.5 GHZ
static const float duree_cycle = (float) 1 / (float) 2.5 ;
// duree du cycle en nano seconde 10^-9

static unsigned long long int residu ;

void init_flop ()
{
  unsigned long long int start, end ;

  start = _rdtsc () ;

  end =_rdtsc () ;

  residu = end - start ;

}


void calcul_flop (char *message, int nb_operations_flottantes, unsigned long long int cycles)
{
  printf ("%s %d operations %5.3f GFLOP/s\n", message, nb_operations_flottantes, ((float) nb_operations_flottantes) / (((float) (cycles - residu)) * duree_cycle)) ;
  return ;
}

double calcul_flop_ret (char *message, int nb_operations_flottantes, unsigned long long int cycles)
{
  float to_ret = ((float) nb_operations_flottantes) / (((float) (cycles - residu)) * duree_cycle);
  //printf ("%s %d operations %5.3f GFLOP/s\n", message, nb_operations_flottantes, to_ret) ;
  return (double) to_ret;
}

void calcul_byte (char *message, int nb_octets_copie, unsigned long long int cycles)
{
  printf ("%s %d operations %5.3f GBytes/s\n", message, nb_octets_copie, ((float) nb_octets_copie) / (((float) (cycles - residu)) * duree_cycle)) ;
  return ;
}

float calcul_byte_swap (char *message, int nb_octets_copie, unsigned long long int cycles)
{
  printf ("%s %d operations %5.3f GBytes/s\n", message, nb_octets_copie, ((float) nb_octets_copie) / (((float) (cycles - residu)) * duree_cycle)) ;
  return ((float) nb_octets_copie) / (((float) (cycles - residu)) * duree_cycle);
}

float calcul_flop_return (char *message, unsigned int nb_operations_flottantes, unsigned long long int cycles)
{
  printf ("%s %d operations %5.3f GFLOP/s\n", message, nb_operations_flottantes, ((float) nb_operations_flottantes) / (((float) (cycles - residu)) * duree_cycle)) ;
  return ((float) nb_operations_flottantes) / (((float) (cycles - residu)) * duree_cycle);
}