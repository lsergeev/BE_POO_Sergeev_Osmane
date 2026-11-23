/*********************************************************************
 * @file  PeripheriquesIoT.h
 * @author L. Sergeev, J. Osmane
 * @brief En-tête unique de la bibliothèque PeripheriquesIoT
 *
 * Hiérarchie :
 *   Peripherique
 *   ├── Capteur ── CapteurUltrason
 *   └── Actionneur ── Led, Buzzer, Servomoteur, MoteurCC ── MoteurBTS7960
 * Exceptions : ErreurPeripherique ── ErreurBroche, ErreurMesure
 *
 * Prérequis : option « C++ Exceptions : Enabled » (Outils de l'IDE).
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_H_
#define PERIPHERIQUES_IOT_H_

#include "Erreurs.h"
#include "Peripherique.h"
#include "Capteur.h"
#include "Actionneur.h"
#include "FiltreMedian.h"
#include "Led.h"
#include "Buzzer.h"
#include "Servomoteur.h"
#include "CapteurUltrason.h"
#include "MoteurCC.h"
#include "MoteurBTS7960.h"

#endif
