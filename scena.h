#pragma once

#include "obiekt.h"

typedef struct
{
  wektor pozycja;
  kolor kolor;
} swiatlo;

typedef struct
{
  int ile_obiektow;
  obiekt* tablica_obiektow;

  int ile_swiatel;
  swiatlo* tablica_swiatel;

  kamera kam;
} scena;

