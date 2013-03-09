#include "kula.h"

#include <stdlib.h>
#include <math.h>

static int kula_przetnij(obiekt* _this, przeciecie** p, promien* pr)
{
  opiskola* dane = (opiskola*)_this->dane;
  wektor eo;
  float v, dist;

  wektor_roznica(&eo, &dane->srodek, pr->poczatek);
  v = wektor_iloczyn_skalarny(&eo, pr->kierunek);
  if (v < 0)
    return 0;
  else
  {
    dist = dane->promien * dane->promien - (wektor_iloczyn_skalarny(&eo, &eo) - v * v);
    if (dist > 0)
    {
      *p = (przeciecie*)malloc(sizeof(**p));
      (*p)->rzecz = _this;
      (*p)->wiazka = *pr;
      (*p)->odleglosc = v - sqrtf(dist);
      return 1;
    }
    else
      return 0;
  }
}

static void kula_normalna(obiekt* _this, wektor* normalna, wektor* pozycja)
{
  wektor_roznica(normalna, pozycja, &((opiskola*)_this->dane)->srodek);
  wektor_normalny(normalna, normalna);
}


void kula_ustaw(obiekt* o, wektor* srodek, float promien, powierzchnia* pow)
{
  opiskola* dane;

  o->przetnij = kula_przetnij;
  o->normalna = kula_normalna;
  o->pow = pow;

  o->dane = (void*)(dane = (opiskola*)malloc(sizeof(*dane)));
  dane->promien = promien;
  dane->srodek = *srodek;
};
