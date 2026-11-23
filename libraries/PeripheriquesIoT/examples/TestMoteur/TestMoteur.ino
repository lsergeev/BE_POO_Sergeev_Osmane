/*********************************************************************
 * @file  TestMoteur.ino
 * @author L. Sergeev, J. Osmane
 * @brief Test de MoteurBTS7960 : rampe, pause avant inversion, arrêt immédiat
 *********************************************************************/
#include <PeripheriquesIoT.h>

// --- Paramètres du test ---
const uint8_t BROCHE_RPWM          = D1;
const uint8_t BROCHE_LPWM          = D2;
const uint8_t BROCHE_ENABLE        = D8;
const uint32_t VITESSE_BAUD        = 115200;
const uint32_t FREQUENCE_PWM_HZ    = 20000;
const uint16_t PENTE_ACCELERATION  = 200;
const uint16_t PENTE_DECELERATION  = 400;
const uint16_t PAUSE_INVERSION_MS  = 300;
const int VITESSE_TEST             = 60;
const uint32_t PERIODE_AFFICHAGE_MS = 100;

/** @brief Une étape de la séquence : consigne pendant une durée */
struct Etape
{
  int consigne;        ///< en %
  uint32_t dureeMs;    ///< durée de l'étape
  bool arretImmediat;  ///< arrêt sans rampe en fin d'étape
};

const Etape SEQUENCE[] = {
  { VITESSE_TEST,  3000, false},
  {-VITESSE_TEST,  3000, false},
  { 0,             2000, false},
  { 100,           1500, true },
  { 0,             2000, false},
};
const size_t NB_ETAPES = sizeof(SEQUENCE) / sizeof(SEQUENCE[0]);

MoteurBTS7960 moteur("propulsion", BROCHE_RPWM, BROCHE_LPWM, BROCHE_ENABLE, FREQUENCE_PWM_HZ,
                     PENTE_ACCELERATION, PENTE_DECELERATION, PAUSE_INVERSION_MS);

bool initialisationReussie = false;
size_t etape = 0;
uint32_t debutEtapeMs = 0;
uint32_t dernierAffichageMs = 0;

void setup()
{
  Serial.begin(VITESSE_BAUD);
  Serial.println();
  Serial.println(F("=== TestMoteur ==="));
  try
  {
    moteur.begin();
    initialisationReussie = true;
  }
  catch (const ErreurPeripherique& e)
  {
    Serial.print(F("Erreur d'initialisation : "));
    Serial.println(e.what());
  }
  Serial << moteur;
  Serial.println();
  debutEtapeMs = millis();
  moteur.setConsigne(SEQUENCE[0].consigne);
}

void loop()
{
  if (!initialisationReussie)
  {
    return;
  }
  const uint32_t maintenant = millis();
  moteur.update(maintenant);

  if (maintenant - debutEtapeMs >= SEQUENCE[etape].dureeMs)
  {
    if (SEQUENCE[etape].arretImmediat)
    {
      moteur.arretImmediat();
      Serial.println(F("Arret immediat"));
    }
    etape = (etape + 1) % NB_ETAPES;
    debutEtapeMs = maintenant;
    moteur.setConsigne(SEQUENCE[etape].consigne);
    Serial << moteur;
    Serial.println();
  }

  if (maintenant - dernierAffichageMs >= PERIODE_AFFICHAGE_MS)
  {
    dernierAffichageMs = maintenant;
    // Format « nom:valeur » compris par le traceur série de l'IDE
    Serial.print(F("consigne:"));
    Serial.print(moteur.consigne());
    Serial.print(F(" vitesse:"));
    Serial.println(moteur.vitesse());
  }
}
