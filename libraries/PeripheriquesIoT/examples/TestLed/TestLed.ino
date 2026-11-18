/*********************************************************************
 * @file  TestLed.ino
 * @author L. Sergeev, J. Osmane
 * @brief Test de la classe Led : LED d'état, phares, feux de recul
 *
 * Attendu :
 *  - LED bleue de la carte : clignote à 1 Hz ;
 *  - phares (D3) et feux de recul (RX) : s'allument en alternance chaque seconde ;
 *  - la carte redémarre normalement avec les LED branchées (bouton RST) ;
 *  - l'inventaire et l'état s'affichent sur le moniteur série (115200 bauds).
 * Brochage identique à projet_voiture_sergeev_osmane/Config.h.
 *********************************************************************/
#include <PeripheriquesIoT.h>

// --- Paramètres du test ---
const uint8_t BROCHE_LED_ETAT   = D4;  // GPIO2, LED interne
const uint8_t BROCHE_PHARES     = D3;  // GPIO0, LED actives à l'état bas
const uint8_t BROCHE_FEUX_RECUL = 3;   // GPIO3 = RX, LED actives à l'état bas
const uint32_t VITESSE_BAUD     = 115200;
const uint32_t PERIODE_LED_ETAT_MS = 1000;
const uint32_t PERIODE_ALTERNANCE_MS = 1000;

Led ledEtat("etat", BROCHE_LED_ETAT, true);
Led phares("phares", BROCHE_PHARES, true);
Led feuxRecul("feux-recul", BROCHE_FEUX_RECUL, true);

uint32_t dernierChangementMs = 0;
bool initialisationReussie = false;

void setup()
{
  // RX sert aux feux de recul : liaison série en émission seule.
  Serial.begin(VITESSE_BAUD, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.println();
  Serial.println(F("=== TestLed ==="));
  try
  {
    ledEtat.begin();
    phares.begin();
    feuxRecul.begin();
    ledEtat.clignoter(PERIODE_LED_ETAT_MS);
    phares.allumer();
    initialisationReussie = true;
  }
  catch (const ErreurPeripherique& e)
  {
    Serial.print(F("Erreur d'initialisation : "));
    Serial.println(e.what());
  }
  Serial << ledEtat << phares << feuxRecul;
  Serial.println();
}

void loop()
{
  if (!initialisationReussie)
  {
    return;
  }
  const uint32_t maintenant = millis();
  ledEtat.update(maintenant);

  if (maintenant - dernierChangementMs >= PERIODE_ALTERNANCE_MS)
  {
    dernierChangementMs = maintenant;
    phares.basculer();
    feuxRecul.commuter(!phares.estAllumee());
    Serial << phares << feuxRecul;
    Serial.println();
  }
}
