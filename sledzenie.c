#include "sledzenie.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <mpi.h>
#include <stdio.h>

g_TrybRownloglosci = BRAK;

static int przetnij(scena* scena, promien* promien, przeciecie** przeciecia)
{
  przeciecie *najblizsze = NULL;
  int i;

  for (i = 0 ; i < scena->ile_obiektow ; i++)
  {
    przeciecie *p;
    if (scena->tablica_obiektow[i].przetnij(&scena->tablica_obiektow[i], &p, promien))
    {
      if (p->odleglosc > 0.001 && (najblizsze == NULL || najblizsze->odleglosc > p->odleglosc))
        najblizsze = p;
    }
  }
  if (przeciecia)
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
        kolor diff = rzecz->pow->diffuse(rzecz->pow, pozycja);
        wektor_iloczyn_float(&lcolor, &s->kolor, illum);
        wektor_iloczyn_osobny(&lcolor, &lcolor, &diff);
        wektor_suma(&ret, &ret, &lcolor);
      }

      wektor_normalny(&tmp, kierunekOdbicia);
      spec = wektor_iloczyn_skalarny(&livec, &tmp);
      if (spec > 0)
      {
        kolor scolor;
        kolor speck = rzecz->pow->specular(rzecz->pow, pozycja);
        wektor_iloczyn_float(&scolor, &s->kolor, powf(spec, rzecz->pow->roughness(rzecz->pow, pozycja)));
        wektor_iloczyn_osobny(&scolor, &scolor, &speck);
        wektor_suma(&ret, &ret, &scolor);
      }
    }
  }

  return ret;
}

static kolor kolor_odbicia(scena* scena, obiekt* rzecz, wektor* pozycja, wektor* normalna, wektor* kierunekOdbicia, int numer_odbicia, int numer_zalamania, float n)
{
  kolor k = {0,0,0,0};
  float reflect = rzecz->pow->reflect(rzecz->pow, pozycja);
  if (reflect == 0.0f)
    return k;
  else
  {
    promien p;
    p.poczatek = pozycja;
    p.kierunek = kierunekOdbicia;

    k = sledz(scena, &p, numer_odbicia + 1, numer_zalamania, n);
    wektor_iloczyn_float(&k, &k, reflect);
    return k;
  }
}

static kolor cieniuj(scena* scena, przeciecie *przec, int numer_odbicia, int numer_zalamania, float n)
{
  kolor ret, pomocniczy;
  wektor pozycja, normalna, odbicie;
  wektor* d = przec->wiazka.kierunek;
  float alpha;
  alpha = przec->rzecz->pow->alpha(przec->rzecz->pow, &pozycja);

  if (alpha > 0.0f)
  {
    // obliczanie wektora normalnego w miejscu przeciecia
    wektor_iloczyn_float(&pozycja, d, przec->odleglosc);
    wektor_suma(&pozycja, &pozycja, przec->wiazka.poczatek);
    przec->rzecz->normalna(przec->rzecz, &normalna, &pozycja);

    // obliczanie wektora odbicia
    wektor_iloczyn_float(&odbicie, &normalna, 2.0f * wektor_iloczyn_skalarny(d, &normalna));
    wektor_roznica(&odbicie, d, &odbicie);

    wektor_ustaw(&ret, 0, 0, 0);

    // standardowe oswietlenie
    pomocniczy = podstawowy_kolor(scena, przec->rzecz, &pozycja, &normalna, &odbicie);
    wektor_suma(&ret, &ret, &pomocniczy);

    // odbicia
    if (numer_odbicia < 5)
    {
      pomocniczy = kolor_odbicia(scena, przec->rzecz, &pozycja, &normalna, &odbicie, numer_odbicia, numer_zalamania, n);
      wektor_suma(&ret, &ret, &pomocniczy);
    }

    wektor_iloczyn_float(&ret, &ret, alpha);
  }

  // obliczanie wektora zalamania
  if (alpha < 1.0f && numer_zalamania < 5)
  {
    float c1, cs2, eta = n / przec->rzecz->pow->n(przec->rzecz->pow, &pozycja);
    c1 = -wektor_iloczyn_skalarny(d, &normalna);
    cs2 = 1.0f - eta * eta * (1 - c1 * c1);
    if (cs2 >= 0.0f)
    {
      przeciecie *p;
      promien prom;
      wektor zalamany, tmp;

      wektor_iloczyn_float(&zalamany, d, eta);
      wektor_iloczyn_float(&tmp, &normalna, eta * c1 - sqrtf(cs2));
      wektor_suma(&zalamany, &zalamany, &tmp);

      prom.poczatek = &pozycja;
      prom.kierunek = &zalamany;

      if (przetnij(scena, &prom, &p))
      {
        kolor dalej = cieniuj(scena, p, numer_odbicia, numer_zalamania + 1, n);
        wektor_iloczyn_float(&dalej, &dalej, 1 - alpha);
        wektor_suma(&ret, &ret, &dalej); 
      }
    }
  }

  return ret;
}

kolor sledz(scena* scena, promien* promien, int numer_odbicia, int numer_zalamania, float n)
{
  przeciecie *przeciecia;
  if (przetnij(scena, promien, &przeciecia)) // > 0 
    return cieniuj(scena, przeciecia, numer_odbicia, numer_zalamania, n);
  else
  {
    kolor k = {0, 0, 0};
    return k;
  }
}

typedef struct
{
  int i;
  float x;
  float y;
  float z;
} ZadanieWyznaczKolorPixela;

static MPI_Datatype MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA;

//#define VERBOSE_PRINT

kolor* generuj(scena* scena, int w, int h, unsigned AA)
{
  int x, y, i, adresat, pozycja;
  float cx, cy;
  const unsigned WIDTH_AA = w * AA;
  const unsigned HEIGHT_AA = h * AA;
  int s = WIDTH_AA * HEIGHT_AA;
  kolor* aktbufor;// = bufor;
  int workers = 1;
  int rank;
  ZadanieWyznaczKolorPixela** listyzadan;
  int* pozycje;
  int ile_max_zadan= (s + workers -1) / workers;
  //int* dlugoscilist

  if (g_TrybRownloglosci == PODZIAL_PIXELI || g_TrybRownloglosci == PODZIAL_PIXELI_DIRICHLET)
    MPI_Comm_size(MPI_COMM_WORLD, &workers);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    listyzadan = (ZadanieWyznaczKolorPixela**)malloc(workers * sizeof(ZadanieWyznaczKolorPixela*));
    pozycje = (int*)malloc(workers * sizeof(int));
    for (i = 0 ; i < workers ; i++)
    {
      pozycje[i] = 0;
      listyzadan[i] = (ZadanieWyznaczKolorPixela*)malloc(ile_max_zadan * sizeof(ZadanieWyznaczKolorPixela));
    }

    aktbufor = (kolor*)malloc(WIDTH_AA * HEIGHT_AA * sizeof(kolor));
#ifdef VERBOSE_PRINT
    printf("Wysy³anie zadan\n");
#endif
    for (i = 0 ; i < s ; i++)
    {
      wektor kierunek, tmp;
      //promien p;
      x = i % WIDTH_AA;
      y = i / HEIGHT_AA;

      cx = (x - (WIDTH_AA / 2.0f)) / (2.0f * WIDTH_AA);
      cy = -(y - (HEIGHT_AA / 2.0f)) / (2.0f * HEIGHT_AA);

      wektor_iloczyn_float(&kierunek, &scena->kam.wprawo, cx);
      wektor_iloczyn_float(&tmp,      &scena->kam.dogory, cy);
      wektor_suma(&kierunek, &kierunek, &tmp);
      wektor_suma(&kierunek, &kierunek, &scena->kam.doprzodu);
      wektor_normalny(&kierunek, &kierunek);

      adresat = 0;
      if (g_TrybRownloglosci == PODZIAL_PIXELI)
        adresat = i / ile_max_zadan;
      /*else if (g_TrybRownloglosci == PODZIAL_PIXELI_DIRICHLET)
        adresat = i % ile_max_zadan;*/

      pozycja = pozycje[adresat]++;
      listyzadan[adresat][pozycja].i = i;
      listyzadan[adresat][pozycja].x = kierunek.x;
      listyzadan[adresat][pozycja].y = kierunek.y;
      listyzadan[adresat][pozycja].z = kierunek.z;

#ifdef VERBOSE_PRINT
      printf("%d [%d]: %f, %f, %f\n", i, adresat, kierunek.x, kierunek.y, kierunek.z);
#endif

      //listyzadan[adresat][pozycja] = kierunek;
    

      //MPI_Isend(&kierunek, 1, MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA, adresat, 0, MPI_COMM_WORLD);
      //p.poczatek = &scena->kam.pozycja;
      //p.kierunek = &kierunek;

      /* zadanie zostanie wykonane w wykonaj_prace()
      aktbufor[i] = sledz(scena, &p, 0, 0, 1.0f);
    
      aktbufor[i].x = aktbufor[i].x > 1 ? 255.0f : (int)(255.0f * aktbufor[i].x);
      aktbufor[i].y = aktbufor[i].y > 1 ? 255.0f : (int)(255.0f * aktbufor[i].y);
      aktbufor[i].z = aktbufor[i].z > 1 ? 255.0f : (int)(255.0f * aktbufor[i].z);
      */
    }
  }

  for (i = 0 ; i < workers ; i++)
  {
    //MPI_Status  status;
    MPI_Request send_request;//, recv_request;

    int ret = MPI_Isend(listyzadan[i], pozycje[i], MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA, i, 0, MPI_COMM_WORLD, &send_request);
  }

  {
    MPI_Status status;
    MPI_Request send_request;
    ZadanieWyznaczKolorPixela* listazadan;
    int dlugosc = s;

    if (g_TrybRownloglosci == PODZIAL_PIXELI)
    {
      dlugosc = ile_max_zadan;
      if ((rank+1) * ile_max_zadan > s)
        dlugosc--;
    }
    /*else if (g_TrybRownloglosci == PODZIAL_PIXELI_DIRICHLET)
    {
      adresat = s % ile_max_zadan;
    }*/

    listazadan = (ZadanieWyznaczKolorPixela*)malloc(dlugosc * sizeof(ZadanieWyznaczKolorPixela));
    // czytamy instrukcje
    MPI_Recv(listazadan, dlugosc, MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA, 0, 0, MPI_COMM_WORLD, &status);

#ifdef VERBOSE_PRINT
    printf("Odbieranie zadan dla adresata %d, dlugosc: %d\n", rank, dlugosc);
#endif

    for (i = 0 ; i < dlugosc ; i++)
    {
      promien p;
      kolor ret;
      p.poczatek = &scena->kam.pozycja;
      p.kierunek = (wektor*)malloc(sizeof(wektor));
      p.kierunek->x = listazadan[i].x;
      p.kierunek->y = listazadan[i].y;
      p.kierunek->z = listazadan[i].z;
#ifdef VERBOSE_PRINT
      printf(" zadanie %d [%d]: %f, %f, %f\n", listazadan[i].i, rank, listazadan[i].x, listazadan[i].y, listazadan[i].z);
#endif
      ret = sledz(scena, &p, 0, 0, 1.0f);

      listazadan[i].x = ret.x > 1 ? 255.0f : (int)(255.0f * ret.x);
      listazadan[i].y = ret.y > 1 ? 255.0f : (int)(255.0f * ret.y);
      listazadan[i].z = ret.z > 1 ? 255.0f : (int)(255.0f * ret.z);
#ifdef VERBOSE_PRINT
      printf(" wynik %d [%d]: %f, %f, %f\n", listazadan[i].i, rank, listazadan[i].x, listazadan[i].y, listazadan[i].z);
#endif
    }

    MPI_Isend(listazadan, dlugosc, MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA, 0, 1, MPI_COMM_WORLD, &send_request);
  }

  if (rank == 0)
  {
    // zbierz wyniki i powklejaj
    for (i = 0 ; i < workers; i++)
    {
      MPI_Status status;
      ZadanieWyznaczKolorPixela* listazadan;
      int dlugosc = s;

      if (g_TrybRownloglosci == PODZIAL_PIXELI)
      {
        dlugosc = ile_max_zadan;
        if ((rank+1) * ile_max_zadan > s)
          dlugosc--;
      }
      listazadan = (ZadanieWyznaczKolorPixela*)malloc(dlugosc * sizeof(ZadanieWyznaczKolorPixela));
      MPI_Recv(listazadan, dlugosc, MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA, i, 1, MPI_COMM_WORLD, &status);

      for (x = 0 ; x < dlugosc ; x++)
      {
        ZadanieWyznaczKolorPixela* z = listazadan + x;
        aktbufor[z->i].x = z->x;
        aktbufor[z->i].y = z->y;
        aktbufor[z->i].z = z->z;
      }
    }

    // handle AA
    if (AA != 1)
    {
      kolor* bufor;
      int x,y, aax, aay;
      bufor = (kolor*)malloc(w * h * sizeof(*bufor));
      for (x = 0 ; x < w ; x++)
      {
        for (y = 0 ; y < h ; y++)
        {
          kolor *r = bufor + h * y + x;
          wektor_ustaw(r, 0,0,0);
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
      return bufor;
    }
    else
      return aktbufor;
  }
  else // if (rank == 0)
    return NULL;
}
/*
void wykonaj_prace()
{
  wektor* listazadan;
  int workers, dlugosc;
  MPI_Comm_size(MPI_COMM_WORLD, &workers);

  // oblicz dlugosc
  if (g_TrybRownloglosci == BRAK)
    dlugosc =
      adresat = 0;
    if (g_TrybRownloglosci == PODZIAL_PIXELI)
      adresat = s / ile_max_zadan;
    else if (g_TrybRownloglosci == PODZIAL_PIXELI_DIRICHLET)
      adresat = s % ile_max_zadan;




  MPI_Recv(listazadan, 
      MPI_Status  status;
    MPI_Request send_request, recv_request;

    int ret = MPI_Isend(listyzadan[i], pozycje[i], MPI_WEKTOR, workers, 0, MPI_COMM_WORLD, &send_request);
}
*/
void sledzenie_init(int* argc, char*** argv)
{
  /*
  struct ZadanieWyznaczKolorPixela
{
  int x;
  int y;
  float x;
  float y;
  float z;
}

static MPI_Datatype MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA;
*/
  MPI_Init(argc, argv); 

  //MPI_Type_contiguous(4, MPI_FLOAT, &MPI_WEKTOR);
  //MPI_Type_commit(&MPI_WEKTOR);
  {
//    ZadanieWyznaczKolorPixela z;
    MPI_Aint disp[4] = { 0, 4, 8, 12};
    MPI_Datatype type[4] = { MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT };
    
    int blocklen[4] = { 1, 1, 1, 1 };
    //disp[0] = (int*)&(z.i) - (int*)&z;
    //disp[1] = (int*)&(z.x) - (int*)&z;
    //disp[2] = (int*)&(z.y) - (int*)&z;
    //disp[3] = (int*)&(z.z) - (int*)&z;

    MPI_Type_create_struct(4, blocklen, disp, type, &MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA);
    MPI_Type_commit(&MPI_ZADANIE_WYZNACZ_KOLOR_PIXELA);

    //MPI_Type_create_struct(
  }
}
