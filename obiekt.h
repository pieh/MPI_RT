#pragma once

#include "wektor.h"

typedef struct
{
  wektor* poczatek;
  wektor* kierunek;
} promien;

typedef struct
{
  struct sobiekt* rzecz;
  promien wiazka;
  float odleglosc;
} przeciecie;

typedef struct
{
  kolor diffuse;
  kolor specular;
  float reflect;
  float roughness;
} powierzchnia;

typedef struct sobiekt
{
  void* dane;
  powierzchnia* pow;

  int (*przetnij)(struct sobiekt* _this, przeciecie** p, promien* pr);
  void (*normalna)(struct sobiekt* _this, wektor* normalna, wektor* pozycja);
} obiekt;