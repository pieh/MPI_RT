#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

#include "obiekt.h"
#include "kula.h"
#include "plaszczyzna.h"
#include "scena.h"
#include "sledzenie.h"
#ifndef _WIN32
#include "zapiszpng.h"
#endif

kolor bialy = {1,1,1,1};
kolor czarny = {0,0,0,0};

kolor szachownica_diffuse(struct spowierzchnia *dane, wektor* p)
{
  return ((int)(floorf(p->z) + floorf(p->x)) % 2) == 0 ? czarny : bialy;
}

static float szachownica_reflect(struct spowierzchnia *dane, wektor* p)
{
  return ((int)(floorf(p->z) + floorf(p->x)) % 2) == 0 ? 1.0f : 0.0f;
}

int main(int argc, char** argv)
{
  int myrank;
  int comm_size;
  const unsigned WIDTH = 600;
  const unsigned HEIGHT = 600;
  danepowierzchnii danepow, danepow2, daneszachownica;
  powierzchnia pow, pow2, szachownica;

  kolor* bufor;
  wektor srodek1 = {0, 1, 0, 0};
  wektor srodek2 = {-1, .5, 1, 0};
  wektor plas_norm = {0, 1, 0, 0};
  wektor poz = {0,1,6,0}, na = {0,1,0,0};
  kolor diff = {1,1,1};
  kolor spec = {.5f, .5f, .5f};

  scena moja_scena;
  clock_t czasomierz;
  float czas;

  MPI_Init(&argc, &argv); 
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  if (myrank != 0)
    goto fin;

  kamera_stworz(&moja_scena.kam, &poz, &na);
  bufor = (kolor*)malloc(WIDTH * HEIGHT * sizeof(*bufor));

  danepow.diffuse = diff;
  danepow.specular = spec;
  danepow.reflect = .60f;
  danepow.roughness = 50;
  danepow.n = 1.2f;
  danepow.alpha = 1.0f;

  danepow2.diffuse = diff;
  danepow2.specular = spec;
  danepow2.reflect = .0f;
  danepow2.roughness = 50;
  danepow2.n = 4.0f/3.0f;
  danepow2.alpha = 0.3f;

  daneszachownica.specular = diff;
  daneszachownica.roughness = 150;
  daneszachownica.n = 1.2f;
  daneszachownica.alpha = 1.0f;

  statyczna_powierzchnia_ustaw(&pow, &danepow);
  statyczna_powierzchnia_ustaw(&pow2, &danepow2);
  statyczna_powierzchnia_ustaw(&szachownica, &daneszachownica);
  szachownica.diffuse = szachownica_diffuse;
  szachownica.reflect = szachownica_reflect;

  moja_scena.ile_obiektow = 3;
  moja_scena.tablica_obiektow = (obiekt*)malloc(moja_scena.ile_obiektow * sizeof(*moja_scena.tablica_obiektow));

  kula_ustaw(&moja_scena.tablica_obiektow[0], &srodek1, 1, &pow);
  kula_ustaw(&moja_scena.tablica_obiektow[1], &srodek2, .5f, &pow2);
  plaszczyzna_ustaw(&moja_scena.tablica_obiektow[2], &plas_norm, 0, &szachownica);

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


  czasomierz = clock();
  generuj(&moja_scena, WIDTH, HEIGHT, bufor, 1);
  czas = (float)(clock() - czasomierz) / CLOCKS_PER_SEC;
  printf("Wygenerowano w czasie %f\n", czas);

#ifndef _WIN32
  char file_name[100];
  time_t t = time(NULL);
  struct tm now = *localtime(&t);

  sprintf(file_name, "RT_out/out %02d-%02d-%02d %02d-%02d-%02d.png",
          now.tm_year + 1900, now.tm_mon + 1, now.tm_mday,
          now.tm_hour, now.tm_min, now.tm_sec); 
  zapisz_png_do_pliku(file_name, bufor, WIDTH, HEIGHT);
#else
  {
    unsigned i;
    FILE* file = fopen("obrazek.txt","w");
    if (file == NULL)
      goto fin;
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
