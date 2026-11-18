/*********************************************************************
 * @file  Application.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Fichier source de l'application
 *********************************************************************/
#include "Application.h"
#include "Config.h"

Application::Application()
  : ledEtat_("etat", config::broches::LED_ETAT, config::eclairage::LED_ETAT_ACTIF_BAS),
    initialisee_(false)
{
}

Application::~Application()
{
}

void Application::init(void)
{
  // RX est réservée aux feux de recul : liaison série en émission seule.
  Serial.begin(config::serie::VITESSE_BAUD, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.println();
  Serial.println(F("=== projet_voiture_sergeev_osmane ==="));

  try
  {
    ledEtat_.begin();
    ledEtat_.clignoter(config::eclairage::PERIODE_ATTENTE_MS);
    initialisee_ = true;
  }
  catch (const ErreurPeripherique& e)
  {
    Serial.print(F("Erreur d'initialisation : "));
    Serial.println(e.what());
    // Dernier recours : la LED interne signale l'erreur si elle est utilisable.
    if (Peripherique::estBrocheValide(config::broches::LED_ETAT))
    {
      pinMode(config::broches::LED_ETAT, OUTPUT);
    }
  }

  Serial << ledEtat_;
  Serial.println();
}

void Application::run(void)
{
  const uint32_t maintenant = millis();
  if (initialisee_)
  {
    ledEtat_.update(maintenant);
  }
  else if (Peripherique::estBrocheValide(config::broches::LED_ETAT))
  {
    // Clignotement rapide d'erreur, sans dépendre d'un objet non initialisé.
    const bool allumee = (maintenant / (config::eclairage::PERIODE_ERREUR_MS / 2)) % 2 == 0;
    digitalWrite(config::broches::LED_ETAT,
                 (allumee != config::eclairage::LED_ETAT_ACTIF_BAS) ? HIGH : LOW);
  }
}
