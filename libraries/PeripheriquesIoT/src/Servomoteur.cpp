/*********************************************************************
 * @file  Servomoteur.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation de la classe Servomoteur
 *********************************************************************/
#include "Servomoteur.h"
#include <algorithm>

Servomoteur::Servomoteur(const char* nom, uint8_t broche, uint8_t angleNeutre, uint8_t amplitude,
                         uint16_t impulsionMinUs, uint16_t impulsionMaxUs)
  : Actionneur(nom, broche),
    angleNeutre_(angleNeutre),
    amplitude_(amplitude),
    impulsionMinUs_(impulsionMinUs),
    impulsionMaxUs_(impulsionMaxUs),
    orientation_(0)
{
}

void Servomoteur::verifierConfiguration() const
{
  exigerPwm(broche_);
  if (angleNeutre_ < amplitude_ || angleNeutre_ + amplitude_ > ANGLE_MAX)
  {
    throw ErreurPeripherique(std::string(nom_) + " : neutre +/- amplitude sort de 0..180 degres");
  }
  if (impulsionMinUs_ >= impulsionMaxUs_)
  {
    throw ErreurPeripherique(std::string(nom_) + " : impulsion min >= impulsion max");
  }
}

void Servomoteur::initialiser()
{
  // La surcharge à 4 paramètres envoie directement l'angle neutre :
  // aucune impulsion à une autre position au démarrage.
  orientation_ = 0;
  servo_.attach(broche_, impulsionMinUs_, impulsionMaxUs_, angleNeutre_);
}

void Servomoteur::orienter(int pourcent)
{
  const int borne = std::clamp(pourcent, -POURCENT_MAX, POURCENT_MAX);
  if (borne == orientation_ && pret_)
  {
    return;
  }
  orientation_ = static_cast<int8_t>(borne);
  if (pret_)
  {
    servo_.write(angle());
  }
}

uint8_t Servomoteur::angle() const
{
  return static_cast<uint8_t>(angleNeutre_ + (orientation_ * amplitude_) / POURCENT_MAX);
}

void Servomoteur::arreter()
{
  orienter(0);
}

void Servomoteur::decrireDetails(Print& sortie) const
{
  sortie.print(" neutre=");
  sortie.print(angleNeutre_);
  sortie.print("deg +/-");
  sortie.print(amplitude_);
  sortie.print(" orientation=");
  sortie.print(orientation_);
  sortie.print('%');
}
