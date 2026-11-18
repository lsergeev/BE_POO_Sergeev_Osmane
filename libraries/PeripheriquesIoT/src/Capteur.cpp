/*********************************************************************
 * @file  Capteur.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation du cadencement des capteurs
 *********************************************************************/
#include "Capteur.h"

Capteur::Capteur(const char* nom, uint8_t broche, uint32_t periodeMs)
  : Peripherique(nom, broche),
    periodeMs_(periodeMs),
    derniereAcquisitionMs_(0),
    premiereAcquisition_(true)
{
}

void Capteur::update(uint32_t maintenantMs)
{
  if (!estPret())
  {
    return;
  }
  // La soustraction non signée reste juste au débordement de millis() (49 jours).
  if (premiereAcquisition_ || (maintenantMs - derniereAcquisitionMs_ >= periodeMs_))
  {
    premiereAcquisition_ = false;
    derniereAcquisitionMs_ = maintenantMs;
    acquerir(maintenantMs);
  }
}
