#ifndef KULA_H
#define KULA_H

#include "obiekt.h"
#include <stdlib.h>
#include <math.h>

typedef struct
{
  float promien;
  wektor srodek;
} opiskola;

void kula_ustaw(obiekt* o, wektor* srodek, float promien, powierzchnia* pow);
#endif