/*********************************************************************
 * @file  Buzzer.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation de la classe Buzzer
 *********************************************************************/
#include "Buzzer.h"
#include <algorithm>

Buzzer::Buzzer(const char* nom, uint8_t broche, uint16_t frequenceHz, uint16_t dureeBipMs)
  : Actionneur(nom, broche),
    frequenceHz_(frequenceHz),
    dureeBipMs_(dureeBipMs),
    fond_(Fond::SILENCE),
    periodeMs_(0),
    debutCycleMs_(0),
    bipPonctuel_(false),
    finBipMs_(0),
    frequenceBipHz_(frequenceHz),
    sonActif_(false),
    frequenceActiveHz_(0)
{
}

void Buzzer::verifierConfiguration() const
{
  // tone() s'appuie sur le générateur de signaux : même contrainte que le PWM.
  exigerPwm(broche_);
  if (frequenceHz_ == 0 || dureeBipMs_ == 0)
  {
    throw ErreurPeripherique(std::string(nom_) + " : frequence et duree de bip doivent etre non nulles");
  }
}

void Buzzer::initialiser()
{
  digitalWrite(broche_, LOW);
  pinMode(broche_, OUTPUT);
  sonActif_ = false;
}

void Buzzer::bip(uint16_t dureeMs)
{
  bip(dureeMs, frequenceHz_);
}

void Buzzer::bip(uint16_t dureeMs, uint16_t frequenceHz)
{
  bipPonctuel_ = true;
  finBipMs_ = millis() + dureeMs;
  frequenceBipHz_ = frequenceHz;
}

void Buzzer::bipPeriodique(uint16_t periodeMs)
{
  if (periodeMs == 0)
  {
    silence();
    return;
  }
  if (fond_ == Fond::PERIODIQUE && periodeMs == periodeMs_)
  {
    return;
  }
  // Nouveau rythme : on conserve la phase si un cycle est déjà en cours,
  // pour que l'accélération du bip reste régulière à l'oreille.
  if (fond_ != Fond::PERIODIQUE)
  {
    debutCycleMs_ = millis();
  }
  fond_ = Fond::PERIODIQUE;
  periodeMs_ = periodeMs;
}

void Buzzer::continu()
{
  fond_ = Fond::CONTINU;
}

void Buzzer::silence()
{
  fond_ = Fond::SILENCE;
}

void Buzzer::arreter()
{
  fond_ = Fond::SILENCE;
  bipPonctuel_ = false;
  appliquer(false, frequenceHz_);
}

void Buzzer::update(uint32_t maintenantMs)
{
  if (!pret_)
  {
    return;
  }
  bool actif = false;
  uint16_t frequence = frequenceHz_;

  // Comparaison signée : reste juste au débordement de millis().
  if (bipPonctuel_ && static_cast<int32_t>(maintenantMs - finBipMs_) >= 0)
  {
    bipPonctuel_ = false;
  }

  if (bipPonctuel_)
  {
    actif = true;
    frequence = frequenceBipHz_;
  }
  else
  {
    switch (fond_)
    {
      case Fond::SILENCE:
        actif = false;
        break;
      case Fond::CONTINU:
        actif = true;
        break;
      case Fond::PERIODIQUE:
      {
        // Le bip ne dépasse jamais la moitié de la période, sinon il serait continu.
        const uint32_t dureeSon = std::min<uint32_t>(dureeBipMs_, periodeMs_ / 2);
        actif = ((maintenantMs - debutCycleMs_) % periodeMs_) < dureeSon;
        break;
      }
    }
  }
  appliquer(actif, frequence);
}

void Buzzer::decrireDetails(Print& sortie) const
{
  sortie.print(' ');
  sortie.print(frequenceHz_);
  sortie.print("Hz");
  switch (fond_)
  {
    case Fond::SILENCE:    sortie.print(" silence"); break;
    case Fond::CONTINU:    sortie.print(" continu"); break;
    case Fond::PERIODIQUE: sortie.print(" bip/"); sortie.print(periodeMs_); sortie.print("ms"); break;
  }
}

void Buzzer::appliquer(bool actif, uint16_t frequenceHz)
{
  if (actif == sonActif_ && (!actif || frequenceHz == frequenceActiveHz_))
  {
    return;
  }
  if (actif)
  {
    tone(broche_, frequenceHz);
  }
  else
  {
    noTone(broche_);
  }
  sonActif_ = actif;
  frequenceActiveHz_ = actif ? frequenceHz : 0;
}
