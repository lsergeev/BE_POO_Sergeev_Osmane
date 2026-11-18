/*********************************************************************
 * @file  CapteurUltrason.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation du télémètre à ultrasons par interruption
 *********************************************************************/
#include "CapteurUltrason.h"

namespace
{
  // Aller-retour du son à 343 m/s : 2 cm / 34 300 cm/s ≈ 58 µs par cm.
  constexpr uint32_t MICROSECONDES_PAR_CM = 58;
  // Impulsion de déclenchement (fiche du Grove Ultrasonic Ranger).
  constexpr uint32_t PAUSE_AVANT_IMPULSION_US = 2;
  constexpr uint32_t DUREE_IMPULSION_US = 10;
}

CapteurUltrason::CapteurUltrason(const char* nom, uint8_t broche, uint32_t periodeMs,
                                 uint16_t porteeMaxCm, uint8_t echecsMax)
  : Capteur(nom, broche, periodeMs),
    filtre_(),
    porteeMaxCm_(porteeMaxCm),
    echecsMax_(echecsMax),
    echecs_(0),
    mesureEnCours_(false),
    debutEchoUs_(0),
    finEchoUs_(0),
    frontMontant_(false),
    echoTermine_(false)
{
}

CapteurUltrason::~CapteurUltrason()
{
  if (pret_)
  {
    detachInterrupt(broche_);
  }
}

void CapteurUltrason::verifierConfiguration() const
{
  // GPIO16 n'a pas d'interruption : même restriction que pour le PWM.
  exigerPwm(broche_);
  if (echecsMax_ == 0 || porteeMaxCm_ == 0)
  {
    throw ErreurPeripherique(std::string(nom_) + " : portee et nombre d'echecs doivent etre non nuls");
  }
}

void CapteurUltrason::initialiser()
{
  pinMode(broche_, INPUT);
  filtre_.vider();
  echecs_ = 0;
  mesureEnCours_ = false;
}

void CapteurUltrason::acquerir(uint32_t /*maintenantMs*/)
{
  if (mesureEnCours_)
  {
    exploiterEcho();
  }
  declencher();
}

void CapteurUltrason::declencher()
{
  // L'interruption est détachée pendant l'impulsion : sinon notre propre
  // signal de déclenchement serait pris pour un écho.
  detachInterrupt(broche_);
  frontMontant_ = false;
  echoTermine_ = false;

  pinMode(broche_, OUTPUT);
  digitalWrite(broche_, LOW);
  delayMicroseconds(PAUSE_AVANT_IMPULSION_US);
  digitalWrite(broche_, HIGH);
  delayMicroseconds(DUREE_IMPULSION_US);
  digitalWrite(broche_, LOW);
  pinMode(broche_, INPUT);

  attachInterruptArg(broche_, &CapteurUltrason::isrEcho, this, CHANGE);
  mesureEnCours_ = true;
}

void CapteurUltrason::exploiterEcho()
{
  // Copie cohérente des variables partagées avec l'ISR.
  noInterrupts();
  const bool termine = echoTermine_;
  const bool montant = frontMontant_;
  const uint32_t debut = debutEchoUs_;
  const uint32_t fin = finEchoUs_;
  interrupts();

  const uint32_t dureeMaxUs = static_cast<uint32_t>(porteeMaxCm_) * MICROSECONDES_PAR_CM;

  if (termine)
  {
    const uint32_t distance = (fin - debut) / MICROSECONDES_PAR_CM;
    filtre_.ajouter(static_cast<uint16_t>(distance > porteeMaxCm_ ? porteeMaxCm_ : distance));
    echecs_ = 0;
  }
  else if (montant && (micros() - debut) >= dureeMaxUs)
  {
    // Écho toujours haut mais déjà plus long que la portée : pas d'obstacle.
    filtre_.ajouter(porteeMaxCm_);
    echecs_ = 0;
  }
  else
  {
    // Pas de front montant : capteur absent ou débranché.
    signalerEchec();
  }
}

void CapteurUltrason::signalerEchec()
{
  if (echecs_ < echecsMax_)
  {
    ++echecs_;
  }
  if (echecs_ >= echecsMax_)
  {
    // Les anciennes valeurs ne décrivent plus la réalité.
    filtre_.vider();
  }
}

bool CapteurUltrason::estFiable() const
{
  return pret_ && echecs_ < echecsMax_ && !filtre_.estVide();
}

uint16_t CapteurUltrason::distanceCm() const
{
  if (!estFiable())
  {
    throw ErreurMesure(std::string(nom_) + " : aucun echo valide ("
                       + std::to_string(echecs_) + " echecs consecutifs)");
  }
  return filtre_.mediane();
}

void CapteurUltrason::decrireDetails(Print& sortie) const
{
  sortie.print(" periode=");
  sortie.print(periodeMs());
  sortie.print("ms portee=");
  sortie.print(porteeMaxCm_);
  sortie.print("cm");
  if (estFiable())
  {
    sortie.print(" distance=");
    sortie.print(filtre_.mediane());
    sortie.print("cm");
  }
  else
  {
    sortie.print(" non-fiable");
  }
}

void IRAM_ATTR CapteurUltrason::isrEcho(void* arg)
{
  CapteurUltrason* capteur = static_cast<CapteurUltrason*>(arg);
  const uint32_t t = micros();
  if (digitalRead(capteur->broche_) == HIGH)
  {
    capteur->debutEchoUs_ = t;
    capteur->frontMontant_ = true;
  }
  else if (capteur->frontMontant_ && !capteur->echoTermine_)
  {
    capteur->finEchoUs_ = t;
    capteur->echoTermine_ = true;
  }
}
