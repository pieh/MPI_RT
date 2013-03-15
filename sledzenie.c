#include "sledzenie.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>

static int przetnij(scena* scena, promien* promien, przeciecie** przeciecia)
{
  przeciecie *najblizsze = NULL;
  int i;

  for (i = 0 ; i < scena->ile_obiektow ; i++)
  {
    przeciecie *p;
    if (scena->tablica_obiektow[i].przetnij(&scena->tablica_obiektow[i], &p, promien))
    {
      if (najblizsze == NULL || najblizsze->odleglosc > p->odleglosc)
        najblizsze = p;
    }
  }
  *przeciecia = najblizsze;
  return najblizsze != NULL;
}

static kolor podstawowy_kolor(scena* scena, obiekt* rzecz, wektor* pozycja, wektor* normalna, wektor* kierunekOdbicia)
{
  kolor ret = {0, 0, 0};
  wektor ldis, livec;
  promien pr;
  int niejestwcieniu;
  int i;

  for (i = 0 ; i < scena->ile_swiatel ; i++)
  {
    przeciecie* p;
    swiatlo* s = &scena->tablica_swiatel[i];
    wektor_roznica(&ldis, &s->pozycja, pozycja);
    wektor_normalny(&livec, &ldis);
    
    pr.poczatek = pozycja;
    pr.kierunek = &livec;

    niejestwcieniu = (przetnij(scena, &pr, &p) == 0 || p->odleglosc > sqrtf(wektor_iloczyn_skalarny(&ldis, &ldis)));
    if (niejestwcieniu)
    {
      float illum, spec;
      wektor tmp;

      illum = wektor_iloczyn_skalarny(&livec, normalna);
      if (illum > 0)
      {
        kolor lcolor;
        wektor_iloczyn_float(&lcolor, &s->kolor, illum);
        wektor_iloczyn_osobny(&lcolor, &lcolor, &rzecz->pow->diffuse);
        wektor_suma(&ret, &ret, &lcolor);
      }

      wektor_normalny(&tmp, kierunekOdbicia);
      spec = wektor_iloczyn_skalarny(&livec, &tmp);
      if (spec > 0)
      {
        kolor scolor;
        wektor_iloczyn_float(&scolor, &s->kolor, powf(spec, rzecz->pow->roughness));
        wektor_iloczyn_osobny(&scolor, &scolor, &rzecz->pow->specular);
        wektor_suma(&ret, &ret, &scolor);
      }
    }
  }

  return ret;
}

static kolor kolor_odbicia(scena* scena, obiekt* rzecz, wektor* pozycja, wektor* normalna, wektor* kierunekOdbicia, int numer_odbicia)
{
  kolor k = {0,0,0,0};
  if (rzecz->pow->reflect == 0.0f)
    return k;
  else
  {
    promien p;
    p.poczatek = pozycja;
    p.kierunek = kierunekOdbicia;

    k = sledz(scena, &p, numer_odbicia + 1);
    wektor_iloczyn_float(&k, &k, rzecz->pow->reflect);
    return k;
  }
}

static kolor cieniuj(scena* scena, przeciecie *przeciecie, int numer_odbicia)
{
  kolor ret, pomocniczy;
  wektor pozycja, normalna, odbicie;

  wektor* d = przeciecie->wiazka.kierunek;
  wektor_iloczyn_float(&pozycja, d, przeciecie->odleglosc);
  wektor_suma(&pozycja, &pozycja, przeciecie->wiazka.poczatek);
  przeciecie->rzecz->normalna(przeciecie->rzecz, &normalna, &pozycja);

  wektor_iloczyn_float(&odbicie, &normalna, 2.0f * wektor_iloczyn_skalarny(d, &normalna));
  wektor_roznica(&odbicie, d, &odbicie);

  wektor_ustaw(&ret, 0, 0, 0);

  // standardowe oswietlenie
  pomocniczy = podstawowy_kolor(scena, przeciecie->rzecz, &pozycja, &normalna, &odbicie);
  wektor_suma(&ret, &ret, &pomocniczy);

  // odbicia
  if (numer_odbicia < 5)
  {
    pomocniczy = kolor_odbicia(scena, przeciecie->rzecz, &pozycja, &normalna, &odbicie, numer_odbicia);
    wektor_suma(&ret, &ret, &pomocniczy);
  }

  return ret;
}

kolor sledz(scena* scena, promien* promien, int numer_odbicia)
{
  przeciecie *przeciecia;
  int ileprzeciec;
  if ((ileprzeciec = przetnij(scena, promien, &przeciecia))) // > 0 
    return cieniuj(scena, przeciecia, numer_odbicia);
  else
  {
    kolor k = {0, 0, 0};
    return k;
  }
}

void generuj(scena* scena, int w, int h, kolor* bufor, unsigned AA)
{
  int x, y, i;
  float cx, cy;
  const unsigned WIDTH_AA = w * AA;
  const unsigned HEIGHT_AA = h * AA;
  int s = WIDTH_AA * HEIGHT_AA;
  kolor* aktbufor = bufor;
  if (AA != 1)
    aktbufor = (kolor*)malloc(WIDTH_AA * HEIGHT_AA * sizeof(*bufor));

  for (i = 0 ; i < s ; i++)
  {
    wektor kierunek, tmp;
    promien p;
    x = i % WIDTH_AA;
    y = i / HEIGHT_AA;

    cx = (x - (WIDTH_AA / 2.0f)) / (2.0f * WIDTH_AA);
    cy = -(y - (HEIGHT_AA / 2.0f)) / (2.0f * HEIGHT_AA);

    wektor_iloczyn_float(&kierunek, &scena->kam.wprawo, cx);
    wektor_iloczyn_float(&tmp,      &scena->kam.dogory, cy);
    wektor_suma(&kierunek, &kierunek, &tmp);
    wektor_suma(&kierunek, &kierunek, &scena->kam.doprzodu);
    wektor_normalny(&kierunek, &kierunek);

    p.poczatek = &scena->kam.pozycja;
    p.kierunek = &kierunek;

    aktbufor[i] = sledz(scena, &p, 0);
    
    aktbufor[i].x = aktbufor[i].x > 1 ? 255.0f : (int)(255.0f * aktbufor[i].x);
    aktbufor[i].y = aktbufor[i].y > 1 ? 255.0f : (int)(255.0f * aktbufor[i].y);
    aktbufor[i].z = aktbufor[i].z > 1 ? 255.0f : (int)(255.0f * aktbufor[i].z);
  }

  if (AA != 1)
  {
    unsigned x, y, aax, aay;
    for (x = 0 ; x < w ; x++)
    {
      for (y = 0 ; y < h ; y++)
      {
        kolor *r = bufor + h * y + x;
        wektor_ustaw(r, 0, 0, 0);
        for (aax = 0 ; aax < AA ; aax++)
        {
          for (aay = 0 ; aay < AA ; aay++)
          {
            wektor_suma(r, r, aktbufor + (x * AA + aax) + WIDTH_AA * (y * AA + aay));
          }
        }
        wektor_iloczyn_float(r, r, 1.0f / (AA * AA));
      }
    }
   free(aktbufor);
  }
}
