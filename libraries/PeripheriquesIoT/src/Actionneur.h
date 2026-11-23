/*********************************************************************
 * @file  Actionneur.h
 * @author L. Sergeev, J. Osmane
 * @brief Classe de base abstraite des actionneurs
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_ACTIONNEUR_H_
#define PERIPHERIQUES_IOT_ACTIONNEUR_H_

#include "Peripherique.h"

/**
 * @class Actionneur
 * @brief Périphérique qui agit sur le monde et sait se mettre en sécurité
 */
class Actionneur : public Peripherique
{
  public:
    using Peripherique::Peripherique;

    /** @brief Ramène l'actionneur dans son état sûr */
    virtual void arreter() = 0;
};

#endif
