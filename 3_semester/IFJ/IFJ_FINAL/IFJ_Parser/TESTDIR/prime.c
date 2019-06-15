/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
 *
 **/

#include <math.h>

#include "prime.h"

// Hladame prvocislo
int isPrime(int x)
{
  if (x < 2)
  {
    return -1;
  }

  if (x < 4)
  {
    return 1;
  }

  if ((x % 2) == 0)
  {
    return 0;
  }

  // Zaokruhlime dole hodnotu sqrt
  int max = floor(sqrt((double) x));
  for (int i = 3; i <= max; i+= 2)
  {
    if ((x % i) == 0)
    {
      return 0;
    }
  }
  return 1;
}

// Najdeme nasledujuce prvocislo po cisle x
int nextPrime(int x)
{
  // Hladame a kontrolujeme ci je prvocislo
  while (isPrime(x) != 1)
  {
    x++;
  }
  // Vratime najdeme prvocislo
  return x;
}
