/*********************************************************************
 * @file  MoteurCC.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation de la rampe et de la pause d'inversion
 *********************************************************************/
#include "MoteurCC.h"
#include <algorithm>
#include <math.h>

namespace
{
  constexpr float MS_PAR_SECONDE = 1000.0f;
}

MoteurCC::MoteurCC(const char* nom, uint8_t broche, uint16_t penteAcceleration,
                   uint16_t penteDeceleration, uint16_t pauseInversionMs)
  : Actionneur(nom, broche),
    consigne_(0),
    vitesse_(0.0f),
    vitesseAppliquee_(0),
    penteAcceleration_(penteAcceleration),
    penteDeceleration_(penteDeceleration),
    pauseInversionMs_(pauseInversionMs),
    dernierUpdateMs_(0),
    premierUpdate_(true),
    dernierSens_(0),
    arretDepuisMs_(0),
    enPause_(false)
{
}

void MoteurCC::verifierConfiguration() const
{
  exigerPwm(broche_);
  if (penteAcceleration_ == 0 || penteDeceleration_ == 0)
  {
    throw ErreurPeripherique(std::string(nom_) + " : les pentes doivent etre non nulles");
  }
}

void MoteurCC::setConsigne(int pourcent)
{
  consigne_ = static_cast<int8_t>(std::clamp(pourcent, -POURCENT_MAX, POURCENT_MAX));
}

void MoteurCC::arreter()
{
  consigne_ = 0;
}

void MoteurCC::arretImmediat()
{
  consigne_ = 0;
  if (vitesse_ != 0.0f)
  {
    arretDepuisMs_ = millis();
  }
  vitesse_ = 0.0f;
  envoyer();
}

void MoteurCC::update(uint32_t maintenantMs)
{
  if (!pret_)
  {
    return;
  }
  const uint32_t dtMs = premierUpdate_ ? 0 : maintenantMs - dernierUpdateMs_;
  dernierUpdateMs_ = maintenantMs;
  premierUpdate_ = false;

  float cible = consigne_;

  // Consigne dans l'autre sens : on commence par revenir à 0.
  if (signe(vitesse_) * signe(cible) < 0)
  {
    cible = 0.0f;
  }

  // À l'arrêt, on ne repart dans l'autre sens qu'après la pause.
  enPause_ = false;
  if (vitesse_ == 0.0f && signe(cible) != 0 && dernierSens_ != 0
      && signe(cible) != dernierSens_ && maintenantMs - arretDepuisMs_ < pauseInversionMs_)
  {
    cible = 0.0f;
    enPause_ = true;
  }

  // Rampe : on accélère si on s'éloigne de 0, on décélère sinon.
  const bool accelere = fabsf(cible) > fabsf(vitesse_);
  const float pente = accelere ? penteAcceleration_ : penteDeceleration_;
  const float pas = pente * dtMs / MS_PAR_SECONDE;
  const float ancienneVitesse = vitesse_;
  if (vitesse_ < cible)
  {
    vitesse_ = std::min(vitesse_ + pas, cible);
  }
  else if (vitesse_ > cible)
  {
    vitesse_ = std::max(vitesse_ - pas, cible);
  }

  if (vitesse_ != 0.0f)
  {
    dernierSens_ = signe(vitesse_);
  }
  else if (ancienneVitesse != 0.0f)
  {
    arretDepuisMs_ = maintenantMs;
  }
  envoyer();
}

void MoteurCC::decrireDetails(Print& sortie) const
{
  sortie.print(" consigne=");
  sortie.print(consigne_);
  sortie.print("% vitesse=");
  sortie.print(vitesseAppliquee_);
  sortie.print('%');
}

int MoteurCC::signe(float valeur)
{
  return (valeur > 0.0f) - (valeur < 0.0f);
}

void MoteurCC::envoyer()
{
  const int8_t arrondi = static_cast<int8_t>(lroundf(vitesse_));
  if (arrondi == vitesseAppliquee_)
  {
    return;
  }
  vitesseAppliquee_ = arrondi;
  if (pret_)
  {
    appliquer(arrondi);
  }
}
