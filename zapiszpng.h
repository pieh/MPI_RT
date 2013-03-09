#ifndef ZAPISZPNG_H
#define ZAPSIZPNG_H

#include <png.h>
#include <stdio.h>
#include "wektor.h"
#include <stdint.h>

int zapisz_png_do_pliku (const char* sciezka, kolor* kolory, size_t w, size_t h);
#endif
