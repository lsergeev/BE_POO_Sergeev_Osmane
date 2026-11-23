/*********************************************************************
 * @file  TestBuzzer.ino
 * @author L. Sergeev, J. Osmane
 * @brief Test de la classe Buzzer, sans aucun delay()
 *********************************************************************/
#include <PeripheriquesIoT.h>

// --- Paramètres du test ---
const uint8_t BROCHE_BUZZER      = D7;    // GPIO13, port D7
const uint32_t VITESSE_BAUD      = 115200;
const uint16_t FREQUENCE_BIP_HZ  = 2000;
const uint16_t DUREE_BIP_MS      = 60;
const uint16_t FREQUENCE_KLAXON_HZ = 440;
const uint16_t DUREE_KLAXON_MS   = 400;
const uint16_t PERIODES_RECUL_MS[] = {500, 250, 120, 100};
const uint32_t DUREE_PALIER_MS   = 2000;
const uint32_t INTERVALLE_BIPS_MS = 300;
const uint8_t NB_BIPS_INITIAUX   = 3;
const uint32_t DUREE_CONTINU_MS  = 1000;
const uint32_t DUREE_SILENCE_MS  = 2000;

Buzzer buzzer("buzzer", BROCHE_BUZZER, FREQUENCE_BIP_HZ, DUREE_BIP_MS);

/** @brief Étapes de la séquence de test */
enum class Etape { BIPS, RECUL, CONTINU, KLAXON, SILENCE };
Etape etape = Etape::BIPS;
uint32_t debutEtapeMs = 0;
uint8_t compteur = 0;
bool initialisationReussie = false;

/** @brief Passe à l'étape suivante et affiche l'état */
void changerEtape(Etape suivante, uint32_t maintenant)
{
  etape = suivante;
  debutEtapeMs = maintenant;
  compteur = 0;
}

void setup()
{
  Serial.begin(VITESSE_BAUD);
  Serial.println();
  Serial.println(F("=== TestBuzzer ==="));
  try
  {
    buzzer.begin();
    initialisationReussie = true;
  }
  catch (const ErreurPeripherique& e)
  {
    Serial.print(F("Erreur d'initialisation : "));
    Serial.println(e.what());
  }
  Serial << buzzer;
  Serial.println();
}

void loop()
{
  if (!initialisationReussie)
  {
    return;
  }
  const uint32_t maintenant = millis();
  const uint32_t ecoule = maintenant - debutEtapeMs;

  switch (etape)
  {
    case Etape::BIPS:
      if (compteur < NB_BIPS_INITIAUX && ecoule >= compteur * INTERVALLE_BIPS_MS)
      {
        buzzer.bip(DUREE_BIP_MS * 2);
        ++compteur;
      }
      else if (compteur >= NB_BIPS_INITIAUX && ecoule >= NB_BIPS_INITIAUX * INTERVALLE_BIPS_MS)
      {
        changerEtape(Etape::RECUL, maintenant);
      }
      break;

    case Etape::RECUL:
    {
      const size_t nbPaliers = sizeof(PERIODES_RECUL_MS) / sizeof(PERIODES_RECUL_MS[0]);
      const size_t palier = ecoule / DUREE_PALIER_MS;
      if (palier < nbPaliers)
      {
        buzzer.bipPeriodique(PERIODES_RECUL_MS[palier]);
        // compteur = numéro du dernier palier affiché + 1 (0 : aucun)
        if (palier + 1 != compteur)
        {
          compteur = static_cast<uint8_t>(palier + 1);
          Serial << buzzer;
          Serial.println();
        }
      }
      else
      {
        buzzer.continu();
        changerEtape(Etape::CONTINU, maintenant);
        Serial << buzzer;
        Serial.println();
      }
      break;
    }

    case Etape::CONTINU:
      if (ecoule >= DUREE_CONTINU_MS)
      {
        buzzer.silence();
        buzzer.bip(DUREE_KLAXON_MS, FREQUENCE_KLAXON_HZ);
        Serial.println(F("Klaxon"));
        changerEtape(Etape::KLAXON, maintenant);
      }
      break;

    case Etape::KLAXON:
      if (ecoule >= DUREE_KLAXON_MS)
      {
        buzzer.arreter();
        changerEtape(Etape::SILENCE, maintenant);
      }
      break;

    case Etape::SILENCE:
      if (ecoule >= DUREE_SILENCE_MS)
      {
        changerEtape(Etape::BIPS, maintenant);
      }
      break;
  }

  buzzer.update(maintenant);
}
