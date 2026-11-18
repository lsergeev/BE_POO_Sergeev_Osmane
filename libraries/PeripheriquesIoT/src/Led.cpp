/*********************************************************************
 * @file  Led.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation de la classe Led
 *********************************************************************/
#include "Led.h"

Led::Led(const char* nom, uint8_t broche, bool actifBas)
  : Actionneur(nom, broche),
    actifBas_(actifBas),
    allumee_(false),
    periodeClignotementMs_(0),
    dernierBasculementMs_(0)
{
}

void Led::initialiser()
{
  // Niveau écrit AVANT le passage en sortie : aucune impulsion parasite,
  // important sur GPIO0/GPIO2 qui ne doivent pas descendre au démarrage.
  digitalWrite(broche_, (allumee_ != actifBas_) ? HIGH : LOW);
  pinMode(broche_, OUTPUT);
}

void Led::allumer()
{
  periodeClignotementMs_ = 0;
  ecrire(true);
}

void Led::eteindre()
{
  periodeClignotementMs_ = 0;
  ecrire(false);
}

void Led::commuter(bool allumee)
{
  periodeClignotementMs_ = 0;
  ecrire(allumee);
}

void Led::basculer()
{
  periodeClignotementMs_ = 0;
  ecrire(!allumee_);
}

void Led::clignoter(uint32_t periodeMs)
{
  if (periodeMs == periodeClignotementMs_)
  {
    return;
  }
  periodeClignotementMs_ = periodeMs;
  dernierBasculementMs_ = millis();
  ecrire(true);
}

void Led::arreter()
{
  eteindre();
}

void Led::update(uint32_t maintenantMs)
{
  if (periodeClignotementMs_ == 0)
  {
    return;
  }
  // Demi-période allumée, demi-période éteinte.
  if (maintenantMs - dernierBasculementMs_ >= periodeClignotementMs_ / 2)
  {
    dernierBasculementMs_ = maintenantMs;
    ecrire(!allumee_);
  }
}

void Led::decrireDetails(Print& sortie) const
{
  if (actifBas_)
  {
    sortie.print(" actif-bas");
  }
  sortie.print(clignote() ? " clignote" : (allumee_ ? " allumee" : " eteinte"));
}

void Led::ecrire(bool allumee)
{
  allumee_ = allumee;
  if (pret_)
  {
    digitalWrite(broche_, (allumee_ != actifBas_) ? HIGH : LOW);
  }
}
