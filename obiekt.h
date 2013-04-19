#ifndef OBIEKT_H
#define OBIEKT_H

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

typedef struct spowierzchnia
{
  kolor (*diffuse)(struct spowierzchnia *dane, wektor* p);
  kolor (*specular)(struct spowierzchnia *dane, wektor* p);
  float (*reflect)(struct spowierzchnia *dane, wektor* p);
  float (*roughness)(struct spowierzchnia *dane, wektor* p);
  float (*n)(struct spowierzchnia *dane, wektor* p);
  float (*alpha)(struct spowierzchnia *dane, wektor* p);

  void* dane;
} powierzchnia;

typedef struct
{
  kolor diffuse;
  kolor specular;
  float reflect;
  float roughness;
  float n; // wzg predkosc w osrodku
  float alpha;
} danepowierzchnii;

void statyczna_powierzchnia_ustaw(powierzchnia* p, danepowierzchnii* dane);

typedef struct sobiekt
{
  void* dane;
  powierzchnia* pow;

  int (*przetnij)(struct sobiekt* _this, przeciecie** p, promien* pr);
  void (*normalna)(struct sobiekt* _this, wektor* normalna, wektor* pozycja);
} obiekt;
#endif
