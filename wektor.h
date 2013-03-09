#ifndef WEKTOR_H
#define WEKTOR_H

#include <math.h>
#include <float.h>
#include <stdlib.h>

// 4 floaty, zeby korzystec z SSE pozniej?
typedef struct
{
  float x, y, z, a;
} wektor;

void wektor_ustaw(wektor* nowy, float _x, float _y, float _z);
void wektor_suma(wektor* suma, wektor* v1, wektor* v2);
void wektor_roznica(wektor* roznica, wektor* v1, wektor* v2);
void wektor_iloczyn_osobny(wektor* iloczyn, wektor* v1, wektor* v2);
void wektor_iloczyn_float(wektor* iloczyn, wektor* v1, float wsp);
float wektor_iloczyn_skalarny(wektor* v1, wektor* v2);
void wektor_normalny(wektor* normalny, wektor* v1);
void wektor_iloczyn_wektorowy(wektor* iloczyn, wektor* v1, wektor* v2);

typedef wektor kolor;

typedef struct
{
  wektor pozycja;
  wektor doprzodu;
  wektor dogory;
  wektor wprawo;
} kamera;

void kamera_stworz(kamera* nowa, wektor* pozycja, wektor* patrz_na);
#endif
