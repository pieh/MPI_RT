#include "plaszczyzna.h"

static int plaszczyzna_przetnij(obiekt* _this, przeciecie** p, promien* pr)
{
  opisplaszczyzny* dane = (opisplaszczyzny*)_this->dane;
  float denom = wektor_iloczyn_skalarny(&dane->normalny, pr->kierunek);
  if (denom <= 0)
  {
    *p = (przeciecie*)malloc(sizeof(**p));
    (*p)->rzecz = _this;
    (*p)->wiazka = *pr;
    (*p)->odleglosc = (wektor_iloczyn_skalarny(&dane->normalny, pr->poczatek) + dane->ofset) / (-denom);
    return 1;
  }
  else
    return 0;
}

static void plaszczyzna_normalna(obiekt* _this, wektor* normalna, wektor* pozycja)
{
  *normalna = ((opisplaszczyzny*)_this->dane)->normalny;
}


void plaszczyzna_ustaw(obiekt* o, wektor* normalny, float ofset, powierzchnia* pow)
{
  opisplaszczyzny* dane;

  o->przetnij = plaszczyzna_przetnij;
  o->normalna = plaszczyzna_normalna;
  o->pow = pow;
  //o->typ = 'S';

  o->dane = (void*)(dane = (opisplaszczyzny*)malloc(sizeof(*dane)));
  dane->ofset = ofset;
  dane->normalny = *normalny;
};