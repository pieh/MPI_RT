#include "obiekt.h"

static kolor diffuse_statyczna(powierzchnia *pow, wektor* p)
{
  return ((danepowierzchnii*)pow->dane)->diffuse;
}

static kolor specular_statyczna(powierzchnia *pow, wektor* p)
{
  return ((danepowierzchnii*)pow->dane)->specular;
}

static float reflect_statyczna(powierzchnia *pow, wektor* p)
{
  return ((danepowierzchnii*)pow->dane)->reflect;
}

static float roughness_statyczna(powierzchnia *pow, wektor* p)
{
  return ((danepowierzchnii*)pow->dane)->roughness;
}

static float n_statyczna(powierzchnia *pow, wektor* p)
{
  return ((danepowierzchnii*)pow->dane)->n;
}

static float alpha_statyczna(powierzchnia *pow, wektor* p)
{
  return ((danepowierzchnii*)pow->dane)->alpha;
}

void statyczna_powierzchnia_ustaw(powierzchnia* p, danepowierzchnii* dane)
{
  p->alpha = alpha_statyczna;
  p->diffuse = diffuse_statyczna;
  p->n = n_statyczna;
  p->reflect = reflect_statyczna;
  p->roughness = roughness_statyczna;
  p->specular = specular_statyczna;
  p->dane = dane;
}