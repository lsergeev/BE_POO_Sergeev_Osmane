/*********************************************************************
 * @file  ServeurPilotage.h
 * @author L. Sergeev, J. Osmane
 * @brief Point d'accès Wi-Fi, page web (HTTP) et commandes (WebSocket)
 *********************************************************************/
#ifndef SERVEUR_PILOTAGE_H_
#define SERVEUR_PILOTAGE_H_

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <functional>
#include "Protocole.h"

/**
 * @class ServeurPilotage
 * @brief Fait le lien entre le téléphone et le reste du programme
 */
class ServeurPilotage
{
  public:
    /** @brief Rappel appelé pour chaque commande valide du conducteur */
    using RappelCommande = std::function<void(const Commande&)>;
    /** @brief Rappel appelé quand le conducteur se déconnecte */
    using RappelDeconnexion = std::function<void()>;

    /** @brief Constructeur (aucun accès au Wi-Fi : objet global possible) */
    ServeurPilotage();

    /**
     * @brief Crée le point d'accès et démarre les serveurs HTTP et WebSocket
     * @throw std::runtime_error si le point d'accès ne peut pas être créé
     */
    void begin();

    /** @brief Traite les requêtes en attente, à appeler à chaque tour de loop() */
    void update();

    /** @brief Branche le rappel des commandes */
    void surCommande(RappelCommande rappel) { surCommande_ = rappel; }
    /** @brief Branche le rappel de déconnexion */
    void surDeconnexion(RappelDeconnexion rappel) { surDeconnexion_ = rappel; }

    /**
     * @brief Envoie l'état au conducteur (s'il y en a un)
     * @param etat état de la voiture
     */
    void diffuser(const EtatVoiture& etat);

    /** @brief Vrai si un conducteur est connecté */
    bool clientConnecte() const { return conducteur_ >= 0; }
    /** @brief Nombre de messages refusés depuis le démarrage */
    uint32_t nbErreurs() const { return nbErreurs_; }

  private:
    /** @brief Événements WebSocket (connexion, déconnexion, texte) */
    void traiterEvenement(uint8_t client, WStype_t type, uint8_t* donnees, size_t longueur);
    /** @brief Traite un message texte du conducteur */
    void traiterMessage(uint8_t client, const uint8_t* donnees, size_t longueur);
    /** @brief Envoie une erreur JSON à un client */
    void envoyerErreur(uint8_t client, const char* raison);

    ESP8266WebServer http_;          ///< sert la page
    WebSocketsServer ws_;            ///< commandes et état
    RappelCommande surCommande_;     ///< vers l'application
    RappelDeconnexion surDeconnexion_;  ///< vers l'application
    int16_t conducteur_;             ///< numéro du client qui pilote, -1 si aucun
    uint32_t nbErreurs_;             ///< messages refusés
};

#endif
