/*********************************************************************
 * @file  TestUltrason.ino
 * @author L. Sergeev, J. Osmane
 * @brief Test de CapteurUltrason (mesure par interruption, filtre médian)
 *
 * Attendu sur le moniteur série (115200 bauds), 10 fois par seconde :
 *  - la distance en cm (à comparer à une règle à 10, 30 et 100 cm) ;
 *  - vers le vide : la portée maximale (300 cm) ;
 *  - câble débranché : « ErreurMesure : ... » SANS redémarrage de la carte,
 *    puis retour des mesures dès que le câble est rebranché.
 * La LED interne bascule à chaque tour de 1000 boucles : si elle
 * clignote vite, loop() n'est jamais bloquée par la mesure.
 * Brochage identique à projet_voiture_sergeev_osmane/Config.h.
 *********************************************************************/
#include <PeripheriquesIoT.h>

// --- Paramètres du test ---
const uint8_t BROCHE_ULTRASON      = D6;  // GPIO12, port D6
const uint8_t BROCHE_LED_ETAT      = D4;  // GPIO2, LED interne
const uint32_t VITESSE_BAUD        = 115200;
const uint32_t PERIODE_MESURE_MS   = 60;
const uint16_t PORTEE_MAX_CM       = 300;
const uint8_t ECHECS_MAX           = 5;
const uint32_t PERIODE_AFFICHAGE_MS = 100;
const uint32_t BOUCLES_PAR_BASCULE = 1000;

CapteurUltrason radar("radar-arriere", BROCHE_ULTRASON, PERIODE_MESURE_MS, PORTEE_MAX_CM, ECHECS_MAX);
Led ledEtat("etat", BROCHE_LED_ETAT, true);

bool initialisationReussie = false;
uint32_t dernierAffichageMs = 0;
uint32_t nbBoucles = 0;

void setup()
{
  Serial.begin(VITESSE_BAUD);
  Serial.println();
  Serial.println(F("=== TestUltrason ==="));
  try
  {
    radar.begin();
    ledEtat.begin();
    initialisationReussie = true;
  }
  catch (const ErreurPeripherique& e)
  {
    Serial.print(F("Erreur d'initialisation : "));
    Serial.println(e.what());
  }
  Serial << radar << ledEtat;
  Serial.println();
}

void loop()
{
  if (!initialisationReussie)
  {
    return;
  }
  const uint32_t maintenant = millis();
  radar.update(maintenant);

  if (++nbBoucles >= BOUCLES_PAR_BASCULE)
  {
    nbBoucles = 0;
    ledEtat.basculer();
  }

  if (maintenant - dernierAffichageMs >= PERIODE_AFFICHAGE_MS)
  {
    dernierAffichageMs = maintenant;
    try
    {
      const uint16_t distance = radar.distanceCm();
      Serial.print(F("distance = "));
      Serial.print(distance);
      Serial.println(F(" cm"));
    }
    catch (const ErreurMesure& e)
    {
      Serial.print(F("ErreurMesure : "));
      Serial.println(e.what());
    }
  }
}
