#include <math.h>

#include "prime.h"


int isPrime(int x)
{
  if (x < 2)
    return -1;

  if (x < 4)
    return 1;

  if ((x % 2) == 0)
    return 0;

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

int nextPrime(int x)
{
  while (isPrime(x) != 1)
  {
    x++;
  }
  return x;
}
