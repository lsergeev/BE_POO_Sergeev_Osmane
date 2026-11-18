/*********************************************************************
 * @file  Peripherique.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation de la classe de base Peripherique
 *********************************************************************/
#include "Peripherique.h"

namespace
{
  // Plan des GPIO de l'ESP8266 (ESP-12E)
  constexpr uint8_t GPIO_MAX = 16;          ///< dernier GPIO existant
  constexpr uint8_t GPIO_FLASH_DEBUT = 6;   ///< GPIO6 à 11 : flash SPI interne
  constexpr uint8_t GPIO_FLASH_FIN = 11;
  constexpr uint8_t GPIO_SANS_PWM = 16;     ///< GPIO16 : ni PWM ni interruption
}

Peripherique::Peripherique(const char* nom, uint8_t broche)
  : nom_(nom), broche_(broche), pret_(false)
{
}

void Peripherique::begin()
{
  pret_ = false;
  verifierConfiguration();
  initialiser();
  pret_ = true;
}

void Peripherique::update(uint32_t /*maintenantMs*/)
{
  // Rien à faire par défaut : les périphériques temporisés redéfinissent update().
}

void Peripherique::decrire(Print& sortie) const
{
  sortie.print('[');
  sortie.print(type());
  sortie.print(" '");
  sortie.print(nom_);
  sortie.print("' GPIO");
  sortie.print(broche_);
  decrireDetails(sortie);
  if (!pret_)
  {
    sortie.print(" non-initialise");
  }
  sortie.print(']');
}

bool Peripherique::estBrocheValide(uint8_t broche)
{
  const bool flash = (broche >= GPIO_FLASH_DEBUT) && (broche <= GPIO_FLASH_FIN);
  return (broche <= GPIO_MAX) && !flash;
}

bool Peripherique::estBrochePwm(uint8_t broche)
{
  return estBrocheValide(broche) && (broche != GPIO_SANS_PWM);
}

void Peripherique::verifierConfiguration() const
{
  exigerBrocheValide(broche_);
}

void Peripherique::decrireDetails(Print& /*sortie*/) const
{
}

void Peripherique::exigerBrocheValide(uint8_t broche) const
{
  if (!estBrocheValide(broche))
  {
    throw ErreurBroche(std::string(nom_) + " : GPIO" + std::to_string(broche)
                       + " inexistante ou reservee a la flash (GPIO6-11)", broche);
  }
}

void Peripherique::exigerPwm(uint8_t broche) const
{
  exigerBrocheValide(broche);
  if (!estBrochePwm(broche))
  {
    throw ErreurBroche(std::string(nom_) + " : GPIO" + std::to_string(broche)
                       + " sans PWM ni interruption (GPIO16)", broche);
  }
}

Print& operator<<(Print& sortie, const Peripherique& p)
{
  p.decrire(sortie);
  return sortie;
}
