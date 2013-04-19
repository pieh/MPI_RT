#ifndef PLASZCZYZNA_H
#define PLASZCZYZNA_H

#include "obiekt.h"

typedef struct
{
  float ofset;
  wektor normalny;
} opisplaszczyzny;


void plaszczyzna_ustaw(obiekt* o, wektor* normalny, float ofset, powierzchnia* pow);
#endif