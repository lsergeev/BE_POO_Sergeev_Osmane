/*********************************************************************
 * @file  Application.h
 * @author L. Sergeev, J. Osmane
 * @brief Fichier header de l'application (chef d'orchestre)
 *
 * Séance 1 : l'application se limite à la LED d'état et à l'inventaire
 * des périphériques. Voiture et ServeurPilotage s'ajoutent aux séances 2 et 3.
 *********************************************************************/
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <PeripheriquesIoT.h>

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
    Led ledEtat_;          ///< LED bleue de la carte : état du système
    bool initialisee_;     ///< vrai si init() a réussi
};
#endif
