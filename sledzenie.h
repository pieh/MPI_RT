#ifndef SLEDZENIE_H
#define SLEDZENIE_H

#include "scena.h"

typedef enum { BRAK, PODZIAL_PIXELI, PODZIAL_PIXELI_DIRICHLET } TrybRownoglosci;

TrybRownoglosci g_TrybRownloglosci;

kolor sledz(scena* scena, promien* promien, int numer_odbicia, int numer_zalamania, float n);
kolor* generuj(scena* scena, int w, int h, unsigned AA);
void wykonaj_prace();
void sledzenie_init(int* argc, char*** argv);
#endif
