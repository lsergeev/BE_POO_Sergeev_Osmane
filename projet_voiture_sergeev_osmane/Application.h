/*********************************************************************
 * @file  Application.h
 * @author L. Sergeev, J. Osmane
 * @brief Fichier header de l'application (chef d'orchestre)
 *
 * Séance 2 : le moteur et la direction sont pilotés depuis le téléphone.
 * La classe Voiture (recul, phares, klaxon) arrive en séance 3.
 *********************************************************************/
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <PeripheriquesIoT.h>
#include "Protocole.h"
#include "ServeurPilotage.h"

/**
  * @class Application
  * @brief Classe Application : initialise et cadence tous les objets du programme
*/
class Application
{
  public :
    /**
     * @fn Application();
     * @brief Constructeur par defaut (aucun accès matériel : objet global)
    */
    Application();
    /**
     * @fn ~Application();
     * @brief Destructeur
    */
    ~Application();
    /**
     * @fn void init(void)
     * @brief Fonction d'initialisation de l'application, appelée par setup()
     *
     * Les exceptions de configuration sont attrapées ici : la carte reste
     * alors dans un état sûr et la LED d'état clignote rapidement.
    */
    void init(void);
    /**
     * @fn void run(void)
     * @brief Fonction de lancement de l'application, appelée par loop() (non bloquante)
    */
    void run(void);

  private :
    /** @brief Applique une commande reçue de la page */
    void traiterCommande(const Commande& commande);

    Led ledEtat_;                 ///< LED bleue de la carte : état du système
    MoteurBTS7960 moteur_;        ///< propulsion
    Servomoteur direction_;       ///< direction
    ServeurPilotage serveur_;     ///< Wi-Fi, page et WebSocket
    Commande derniereCommande_;   ///< pour n'afficher que les changements
    bool initialisee_;            ///< vrai si init() a réussi
};
#endif
