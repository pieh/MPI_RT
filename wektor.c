#include "wektor.h"

#include <float.h>
#include <stdlib.h>
#include <math.h>

void wektor_ustaw(wektor* nowy, float _x, float _y, float _z)
{
  nowy->x = _x;
  nowy->y = _y;
  nowy->z = _z;
  nowy->a = 0.0f;
}

void wektor_suma(wektor* suma, wektor* v1, wektor* v2)
{
  suma->x = v1->x + v2->x;
  suma->y = v1->y + v2->y;
  suma->z = v1->z + v2->z;
  suma->a = 0.0f;
}

void wektor_roznica(wektor* roznica, wektor* v1, wektor* v2)
{
  roznica->x = v1->x - v2->x;
  roznica->y = v1->y - v2->y;
  roznica->z = v1->z - v2->z;
  roznica->a = 0.0f;
}

void wektor_iloczyn_osobny(wektor* iloczyn, wektor* v1, wektor* v2)
{
  iloczyn->x = v1->x * v2->x;
  iloczyn->y = v1->y * v2->y;
  iloczyn->z = v1->z * v2->z;
  iloczyn->a = 0.0f;
}

void wektor_iloczyn_float(wektor* iloczyn, wektor* v1, float wsp)
{
  iloczyn->x = v1->x * wsp;
  iloczyn->y = v1->y * wsp;
  iloczyn->z = v1->z * wsp;
  iloczyn->a = 0.0f;
}

float wektor_iloczyn_skalarny(wektor* v1, wektor* v2)
{
  return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

void wektor_normalny(wektor* normalny, wektor* v1)
{
  float l = sqrtf(wektor_iloczyn_skalarny(v1,v1));
  wektor_iloczyn_float(normalny, v1, 1.0f / (l != 0.0f ? l : FLT_MAX));
}

void wektor_iloczyn_wektorowy(wektor* iloczyn, wektor* v1, wektor* v2)
{
  float y = v1->z * v2->x - v1->x * v2->z;
  float z = v1->x * v2->y - v1->y * v2->x;

  iloczyn->x = (v1->y * v2->z) - (v1->z * v2->y);
  iloczyn->y = y;
  iloczyn->z = z;
  iloczyn->a = 0.0f;
}

void kamera_stworz(kamera* nowa, wektor* pozycja, wektor* patrz_na)
{
  wektor tmp, dol;

  nowa->pozycja = *pozycja;

  wektor_roznica(&tmp, patrz_na, pozycja);
  wektor_normalny(&nowa->doprzodu, &tmp);

  wektor_ustaw(&dol, 0.0f, -1.0f, 0.0f);
  wektor_iloczyn_wektorowy(&tmp, &nowa->doprzodu, &dol);
  wektor_normalny(&tmp, &tmp);
  wektor_iloczyn_float(&nowa->wprawo, &tmp, 1.5f);

  wektor_iloczyn_wektorowy(&tmp, &nowa->doprzodu, &nowa->wprawo);
  wektor_normalny(&tmp, &tmp);
  wektor_iloczyn_float(&nowa->dogory, &tmp, 1.5f);
}
