#ifndef FLOP_H
#define FLOP_H

void init_flop () ;

void calcul_flop (char *message, int nb_operations_flottantes, unsigned long long int cycles) ;

double calcul_flop_ret (char *message, int nb_operations_flottantes, unsigned long long int cycles) ;

void calcul_byte (char *message, int nb_octets_copie, unsigned long long int cycles);

float calcul_byte_swap (char *message, int nb_octets_copie, unsigned long long int cycles);

float calcul_flop_return (char *message, unsigned int nb_operations_flottantes, unsigned long long int cycles);


#endif
