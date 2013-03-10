#include <stdio.h>
#include <stdint.h>
#include <mpi.h>

#include "obiekt.h"
#include "kula.h"
#include "scena.h"
#include "sledzenie.h"

#ifndef _WIN32
#include "zapiszpng.h"
#endif

int main(int argc, char** argv)
{
  int myrank;
  int comm_size;
  const unsigned WIDTH = 600;
  const unsigned HEIGHT = 600;
  powierzchnia pow;
  kolor* bufor;
  wektor srodek1 = {0, 1, 0, 0};
  wektor srodek2 = {-1, .5, 1.5, 0};
  wektor srodek3 = {0.4, 0.3, 1, 0};
  wektor poz = {3,2,4,0}, na = {-1,.5,0,0};
  //wektor poz = {0, 0, 5, 0}, na = {0, 0, 0 ,0};
  kolor diff = {1, 1, 1};
  kolor spec = {.5, .5, .5};
  kolor ambient = {0, 0.1, 0};
  scena moja_scena;

  MPI_Init(&argc, &argv); 
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  if (myrank != 0)
    goto fin;

  kamera_stworz(&moja_scena.kam, &poz, &na);
  bufor = (kolor*)malloc(WIDTH * HEIGHT * sizeof(*bufor));

  pow.diffuse = diff;
  pow.specular = spec;
  pow.reflect = .6f;
  pow.roughness = 50;
  pow.ambient = ambient;

  moja_scena.ile_obiektow = 2;
  moja_scena.tablica_obiektow = (obiekt*)malloc(moja_scena.ile_obiektow * sizeof(*moja_scena.tablica_obiektow));

  kula_ustaw(&moja_scena.tablica_obiektow[0], &srodek1, 1, &pow);
  kula_ustaw(&moja_scena.tablica_obiektow[1], &srodek2, .5, &pow);
  //kula_ustaw(&moja_scena.tablica_obiektow[2], &srodek3, 0.75f, &pow);

  moja_scena.ile_swiatel = 4;
  moja_scena.tablica_swiatel = (swiatlo*)malloc(moja_scena.ile_swiatel * sizeof(*moja_scena.tablica_swiatel));
  
  wektor_ustaw(&moja_scena.tablica_swiatel[0].pozycja, -2, 2.5, 0);
  wektor_ustaw(&moja_scena.tablica_swiatel[0].kolor, .49f, .07f, .07f);

  wektor_ustaw(&moja_scena.tablica_swiatel[1].pozycja, 1.5, 2.5, 1.5);
  wektor_ustaw(&moja_scena.tablica_swiatel[1].kolor, .07f, .07f, .49f);

  wektor_ustaw(&moja_scena.tablica_swiatel[2].pozycja, 1.5, 2.5, -1.5);
  wektor_ustaw(&moja_scena.tablica_swiatel[2].kolor, .07f, .49f, .071f);

  wektor_ustaw(&moja_scena.tablica_swiatel[3].pozycja, 0, 3.5, 0);
  wektor_ustaw(&moja_scena.tablica_swiatel[3].kolor, .21f, .21f, .35f);

  generuj(&moja_scena, WIDTH, HEIGHT, bufor);
  printf("Wygenerowano\n");

#ifndef _WIN32
  zapisz_png_do_pliku("obr.png", bufor, WIDTH, HEIGHT);
#else
  {
    unsigned i;
    FILE* file = fopen("obrazek.txt","w");
    for (i = 0 ; i < WIDTH * HEIGHT ; i++)
    {
      fprintf(file, "%d %d %d ", (uint8_t)bufor[i].x, (uint8_t)bufor[i].y, (uint8_t)bufor[i].z);
      if (i % WIDTH == WIDTH - 1)
        fprintf(file, "\n");
    }
  }
#endif

fin:
  MPI_Finalize(); 

  return 0;
}