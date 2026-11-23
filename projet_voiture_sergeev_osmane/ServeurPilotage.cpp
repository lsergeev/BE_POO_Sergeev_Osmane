/*********************************************************************
 * @file  ServeurPilotage.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Implémentation du serveur de pilotage
 *********************************************************************/
#include "ServeurPilotage.h"
#include <ESP8266WiFi.h>
#include <stdexcept>
#include "Config.h"
#include "PageWeb.h"

namespace
{
  constexpr int16_t AUCUN_CONDUCTEUR = -1;
  constexpr int CODE_HTTP_OK = 200;
}

ServeurPilotage::ServeurPilotage()
  : http_(config::reseau::PORT_HTTP),
    ws_(config::reseau::PORT_WEBSOCKET),
    surCommande_(),
    surDeconnexion_(),
    conducteur_(AUCUN_CONDUCTEUR),
    nbErreurs_(0)
{
}

void ServeurPilotage::begin()
{
  using namespace config::reseau;
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(SSID, MOT_DE_PASSE, CANAL, false, CLIENTS_MAX))
  {
    throw std::runtime_error("impossible de creer le point d'acces Wi-Fi");
  }

  // La page est la même quelle que soit l'adresse demandée.
  auto envoyerPage = [this]() { http_.send_P(CODE_HTTP_OK, "text/html", PAGE_WEB); };
  http_.on("/", envoyerPage);
  http_.onNotFound(envoyerPage);
  http_.begin();

  ws_.onEvent([this](uint8_t client, WStype_t type, uint8_t* donnees, size_t longueur) {
    traiterEvenement(client, type, donnees, longueur);
  });
  ws_.begin();

  Serial.print(F("Point d'acces "));
  Serial.print(SSID);
  Serial.print(F(" sur http://"));
  Serial.println(WiFi.softAPIP());
}

void ServeurPilotage::update()
{
  http_.handleClient();
  ws_.loop();
}

void ServeurPilotage::diffuser(const EtatVoiture& etat)
{
  if (conducteur_ == AUCUN_CONDUCTEUR)
  {
    return;
  }
  char tampon[Protocole::TAILLE_TAMPON];
  const size_t longueur = Protocole::encoder(etat, tampon, sizeof(tampon));
  ws_.sendTXT(static_cast<uint8_t>(conducteur_), tampon, longueur);
}

void ServeurPilotage::traiterEvenement(uint8_t client, WStype_t type, uint8_t* donnees, size_t longueur)
{
  switch (type)
  {
    case WStype_CONNECTED:
      if (conducteur_ != AUCUN_CONDUCTEUR && conducteur_ != client)
      {
        envoyerErreur(client, "occupe");
        ws_.disconnect(client);
        return;
      }
      conducteur_ = client;
      Serial.println(F("Conducteur connecte"));
      break;

    case WStype_DISCONNECTED:
      if (client == conducteur_)
      {
        conducteur_ = AUCUN_CONDUCTEUR;
        Serial.println(F("Conducteur deconnecte"));
        if (surDeconnexion_)
        {
          surDeconnexion_();
        }
      }
      break;

    case WStype_TEXT:
      if (client == conducteur_)
      {
        traiterMessage(client, donnees, longueur);
      }
      break;

    default:
      break;
  }
}

void ServeurPilotage::traiterMessage(uint8_t client, const uint8_t* donnees, size_t longueur)
{
  // Copie dans un tampon à nous, terminé par '\0'. Un message trop long
  // est tronqué, et decoder() le refusera.
  char texte[config::protocole::LONGUEUR_MAX + 2];
  const size_t n = std::min(longueur, sizeof(texte) - 1);
  memcpy(texte, donnees, n);
  texte[n] = '\0';

  try
  {
    const Commande commande = Protocole::decoder(texte);
    if (surCommande_)
    {
      surCommande_(commande);
    }
  }
  catch (const ErreurProtocole& e)
  {
    ++nbErreurs_;
    Serial.print(F("Message refuse : "));
    Serial.println(e.what());
    envoyerErreur(client, e.what());
  }
}

void ServeurPilotage::envoyerErreur(uint8_t client, const char* raison)
{
  char tampon[Protocole::TAILLE_TAMPON];
  const size_t longueur = Protocole::encoderErreur(raison, tampon, sizeof(tampon));
  if (longueur > 0)
  {
    ws_.sendTXT(client, tampon, longueur);
  }
}
