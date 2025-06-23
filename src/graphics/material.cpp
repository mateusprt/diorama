#include "material.h"

using namespace std;

// implementação do construtor completo
Material::Material(
  glm::vec3 Ka,
  glm::vec3 Kd,
  glm::vec3 Ks,
  float Ns,
  string map_Kd)
    : Ka(Ka)
    , Kd(Kd)
    , Ks(Ks)
    , Ns(Ns)
    , map_Kd(std::move(map_Kd))
{
}