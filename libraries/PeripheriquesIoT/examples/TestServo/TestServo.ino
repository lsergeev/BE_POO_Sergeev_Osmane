/*********************************************************************
 * @file  TestServo.ino
 * @author L. Sergeev, J. Osmane
 * @brief Test et calibration de la classe Servomoteur
 *********************************************************************/
#include <PeripheriquesIoT.h>

// --- Paramètres du test (à calibrer) ---
const uint8_t BROCHE_SERVO        = D5;  // GPIO14, port D5
const uint32_t VITESSE_BAUD       = 115200;
const uint8_t ANGLE_NEUTRE        = 90;
const uint8_t AMPLITUDE           = 35;
const uint16_t IMPULSION_MIN_US   = 1000;
const uint16_t IMPULSION_MAX_US   = 2000;
const uint32_t ATTENTE_NEUTRE_MS  = 3000;
const uint32_t PAS_BALAYAGE_MS    = 20;   // 1 % toutes les 20 ms : aller en 4 s
const uint32_t PERIODE_AFFICHAGE_MS = 500;

Servomoteur direction("direction", BROCHE_SERVO, ANGLE_NEUTRE, AMPLITUDE,
                      IMPULSION_MIN_US, IMPULSION_MAX_US);

bool initialisationReussie = false;
bool balayage = true;
int pourcent = 0;
int sens = 1;
uint32_t dernierPasMs = 0;
uint32_t dernierAffichageMs = 0;
String saisie;

void setup()
{
  Serial.begin(VITESSE_BAUD);
  Serial.println();
  Serial.println(F("=== TestServo ==="));
  try
  {
    direction.begin();
    initialisationReussie = true;
  }
  catch (const ErreurPeripherique& e)
  {
    Serial.print(F("Erreur d'initialisation : "));
    Serial.println(e.what());
  }
  Serial << direction;
  Serial.println();
}

/** @brief Lit une commande complète sur le port série, sans bloquer */
void lireCommande()
{
  while (Serial.available() > 0)
  {
    const char c = static_cast<char>(Serial.read());
    if (c != '\n' && c != '\r')
    {
      saisie += c;
      continue;
    }
    saisie.trim();
    if (saisie == "b")
    {
      balayage = true;
      Serial.println(F("Balayage"));
    }
    else if (saisie.length() > 0)
    {
      balayage = false;
      direction.orienter(saisie.toInt());
      Serial << direction;
      Serial.println();
    }
    saisie = "";
  }
}

void loop()
{
  if (!initialisationReussie)
  {
    return;
  }
  const uint32_t maintenant = millis();
  lireCommande();

  if (balayage && maintenant >= ATTENTE_NEUTRE_MS && maintenant - dernierPasMs >= PAS_BALAYAGE_MS)
  {
    dernierPasMs = maintenant;
    pourcent += sens;
    if (pourcent >= Servomoteur::POURCENT_MAX || pourcent <= -Servomoteur::POURCENT_MAX)
    {
      sens = -sens;
    }
    direction.orienter(pourcent);
  }

  if (balayage && maintenant - dernierAffichageMs >= PERIODE_AFFICHAGE_MS)
  {
    dernierAffichageMs = maintenant;
    Serial.print(F("angle="));
    Serial.println(direction.angle());
  }
}
