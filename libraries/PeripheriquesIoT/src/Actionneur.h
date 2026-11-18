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
 *
 * arreter() ramène l'actionneur dans son état sûr (LED éteinte, buzzer
 * muet, servo au neutre, moteur arrêté). Une application peut ainsi
 * mettre tout un système en sécurité en parcourant ses actionneurs,
 * sans connaître leur type exact.
 */
class Actionneur : public Peripherique
{
  public:
    using Peripherique::Peripherique;

    /** @brief Ramène l'actionneur dans son état sûr */
    virtual void arreter() = 0;
};

#endif
