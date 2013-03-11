#ifndef SLEDZENIE_H
#define SLEDZENIE_H

#include "scena.h"

kolor sledz(scena* scena, promien* promien, int numer_odbicia, int numer_zalamania, float n);
void generuj(scena* scena, int w, int h, kolor* bufor, unsigned AA);
#endif
