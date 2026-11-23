/*********************************************************************
 * @file  Application.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Fichier source de l'application
 *********************************************************************/
#include "Application.h"
#include "Config.h"

Application::Application()
  : ledEtat_("etat", config::broches::LED_ETAT, config::eclairage::LED_ETAT_ACTIF_BAS),
    moteur_("propulsion", config::broches::MOTEUR_RPWM, config::broches::MOTEUR_LPWM,
            config::broches::MOTEUR_ENABLE, config::moteur::FREQUENCE_PWM_HZ,
            config::moteur::PENTE_ACCELERATION, config::moteur::PENTE_DECELERATION,
            config::moteur::PAUSE_INVERSION_MS),
    direction_("direction", config::broches::SERVO, config::direction::ANGLE_NEUTRE,
               config::direction::AMPLITUDE, config::direction::IMPULSION_MIN_US,
               config::direction::IMPULSION_MAX_US),
    serveur_(),
    derniereCommande_(),
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
    direction_.begin();   // roues droites dès la première impulsion
    moteur_.begin();
    serveur_.surCommande([this](const Commande& commande) { traiterCommande(commande); });
    serveur_.begin();
    ledEtat_.clignoter(config::eclairage::PERIODE_ATTENTE_MS);
    initialisee_ = true;
  }
  catch (const std::exception& e)
  {
    // ErreurPeripherique (broche, config) ou erreur du Wi-Fi : on ne pilote pas.
    Serial.print(F("Erreur d'initialisation : "));
    Serial.println(e.what());
    if (Peripherique::estBrocheValide(config::broches::LED_ETAT))
    {
      pinMode(config::broches::LED_ETAT, OUTPUT);
    }
  }

  Serial << ledEtat_ << direction_ << moteur_;
  Serial.println();
}

void Application::run(void)
{
  const uint32_t maintenant = millis();
  if (initialisee_)
  {
    serveur_.update();
    moteur_.update(maintenant);
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

void Application::traiterCommande(const Commande& commande)
{
  if (commande != derniereCommande_)
  {
    Serial.printf("Commande %d : v=%d d=%d val=%u\n", static_cast<int>(commande.type),
                  commande.vitesse, commande.direction, commande.valeur);
    derniereCommande_ = commande;
  }

  switch (commande.type)
  {
    case TypeCommande::CONDUITE:
      moteur_.setConsigne(commande.vitesse);
      direction_.orienter(commande.direction);
      break;
    default:
      // Phares, klaxon et seuil : avec la classe Voiture (séance 3).
      break;
  }
}
