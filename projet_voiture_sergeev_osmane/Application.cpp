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
    derniereCommandeMs_(0),
    dernierEtatMs_(0),
    enSecurite_(true),
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
    serveur_.surDeconnexion([this]() { miseEnSecurite(); });
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

    // Chien de garde : la page envoie une commande toutes les 100 ms.
    // Plus rien depuis 500 ms (téléphone verrouillé, Wi-Fi coupé...) : on arrête.
    if (!enSecurite_ && maintenant - derniereCommandeMs_ >= config::securite::TIMEOUT_COMMANDE_MS)
    {
      Serial.println(F("Plus de commande : arret"));
      miseEnSecurite();
    }

    moteur_.update(maintenant);

    // LED fixe quand quelqu'un pilote, clignotement lent sinon
    if (serveur_.clientConnecte())
    {
      ledEtat_.allumer();
    }
    else
    {
      ledEtat_.clignoter(config::eclairage::PERIODE_ATTENTE_MS);
    }
    ledEtat_.update(maintenant);

    if (maintenant - dernierEtatMs_ >= config::reseau::PERIODE_ETAT_MS)
    {
      dernierEtatMs_ = maintenant;
      envoyerEtat();
    }
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
      derniereCommandeMs_ = millis();
      enSecurite_ = false;
      moteur_.setConsigne(commande.vitesse);
      direction_.orienter(commande.direction);
      break;
    default:
      // Phares, klaxon et seuil : avec la classe Voiture (séance 3).
      break;
  }
}

void Application::miseEnSecurite()
{
  enSecurite_ = true;
  moteur_.arreter();     // consigne à 0, la rampe de freinage s'occupe du reste
  direction_.arreter();  // roues droites
}

ModeConduite Application::mode() const
{
  if (enSecurite_ && serveur_.clientConnecte())
  {
    return ModeConduite::SECURITE;
  }
  if (moteur_.vitesse() > 0)
  {
    return ModeConduite::AVANT;
  }
  if (moteur_.vitesse() < 0)
  {
    return ModeConduite::ARRIERE;
  }
  return ModeConduite::ARRET;
}

void Application::envoyerEtat()
{
  EtatVoiture etat;
  etat.vitesse = moteur_.vitesse();
  etat.consigne = moteur_.consigne();
  etat.direction = direction_.orientation();
  etat.mode = mode();
  // Pas encore de capteur arrière ni d'éclairage dans l'appli (séance 3)
  serveur_.diffuser(etat);
}
