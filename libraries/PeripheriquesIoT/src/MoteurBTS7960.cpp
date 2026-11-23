/*********************************************************************
 * @file  MoteurBTS7960.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation du pilote BTS7960
 *********************************************************************/
#include "MoteurBTS7960.h"
#include <stdlib.h>

namespace
{
  constexpr uint32_t FREQUENCE_PWM_MAX_HZ = 25000;  ///< limite du BTS7960
}

MoteurBTS7960::MoteurBTS7960(const char* nom, uint8_t brocheRpwm, uint8_t brocheLpwm,
                             uint8_t brocheEnable, uint32_t frequencePwmHz,
                             uint16_t penteAcceleration, uint16_t penteDeceleration,
                             uint16_t pauseInversionMs)
  : MoteurCC(nom, brocheRpwm, penteAcceleration, penteDeceleration, pauseInversionMs),
    brocheLpwm_(brocheLpwm),
    brocheEnable_(brocheEnable),
    frequencePwmHz_(frequencePwmHz),
    active_(false)
{
}

void MoteurBTS7960::verifierConfiguration() const
{
  MoteurCC::verifierConfiguration();  // RPWM et pentes
  exigerPwm(brocheLpwm_);
  exigerBrocheValide(brocheEnable_);
  if (broche_ == brocheLpwm_ || broche_ == brocheEnable_ || brocheLpwm_ == brocheEnable_)
  {
    throw ErreurBroche(std::string(nom_) + " : RPWM, LPWM et EN doivent etre sur 3 broches differentes",
                       brocheEnable_);
  }
  if (frequencePwmHz_ == 0 || frequencePwmHz_ > FREQUENCE_PWM_MAX_HZ)
  {
    throw ErreurPeripherique(std::string(nom_) + " : frequence PWM hors de 1..25000 Hz");
  }
}

void MoteurBTS7960::initialiser()
{
  // D'abord le pont coupé, puis les PWM à 0, et seulement ensuite on active.
  digitalWrite(brocheEnable_, LOW);
  pinMode(brocheEnable_, OUTPUT);
  analogWriteRange(PLAGE_PWM);
  analogWriteFreq(frequencePwmHz_);
  pinMode(broche_, OUTPUT);
  pinMode(brocheLpwm_, OUTPUT);
  analogWrite(broche_, 0);
  analogWrite(brocheLpwm_, 0);
  activer();
}

void MoteurBTS7960::desactiver()
{
  arretImmediat();
  active_ = false;
  digitalWrite(brocheEnable_, LOW);
}

void MoteurBTS7960::activer()
{
  active_ = true;
  digitalWrite(brocheEnable_, HIGH);
}

void MoteurBTS7960::appliquer(int8_t vitesse)
{
  const uint32_t rapport = static_cast<uint32_t>(abs(vitesse)) * PLAGE_PWM / POURCENT_MAX;
  // On met toujours à 0 la broche du sens opposé AVANT d'écrire l'autre.
  if (vitesse > 0)
  {
    analogWrite(brocheLpwm_, 0);
    analogWrite(broche_, rapport);
  }
  else if (vitesse < 0)
  {
    analogWrite(broche_, 0);
    analogWrite(brocheLpwm_, rapport);
  }
  else
  {
    analogWrite(broche_, 0);
    analogWrite(brocheLpwm_, 0);
  }
}

void MoteurBTS7960::decrireDetails(Print& sortie) const
{
  sortie.print(" LPWM=GPIO");
  sortie.print(brocheLpwm_);
  sortie.print(" EN=GPIO");
  sortie.print(brocheEnable_);
  sortie.print(active_ ? " actif" : " coupe");
  MoteurCC::decrireDetails(sortie);
}
