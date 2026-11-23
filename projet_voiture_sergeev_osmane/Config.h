/*********************************************************************
 * @file  Config.h
 * @author L. Sergeev, J. Osmane
 * @brief Toutes les constantes de l'application : broches, seuils, délais, Wi-Fi
 *
 * Aucune valeur numérique ne doit apparaître ailleurs dans l'application.
 * La bibliothèque PeripheriquesIoT n'inclut jamais ce fichier : elle
 * reçoit ces valeurs par les constructeurs.
 * Justification du brochage : docs/cablage.md
 *********************************************************************/
#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

namespace config
{
  /** @brief Affectation des broches (NodeMCU 1.0 + shield Grove) */
  namespace broches
  {
    constexpr uint8_t MOTEUR_RPWM   = D1;  ///< GPIO5, port I2C fil jaune : marche avant
    constexpr uint8_t MOTEUR_LPWM   = D2;  ///< GPIO4, port I2C fil blanc : marche arrière
    constexpr uint8_t MOTEUR_ENABLE = D8;  ///< GPIO15, R_EN+L_EN : forcée à 0 au boot
    constexpr uint8_t SERVO         = D5;  ///< GPIO14, port D5
    constexpr uint8_t ULTRASON      = D6;  ///< GPIO12, port D6 (interruption)
    constexpr uint8_t BUZZER        = D7;  ///< GPIO13, port D7 (jamais sur D3 : boot)
    constexpr uint8_t PHARES        = D3;  ///< GPIO0, LED câblées actives à l'état bas
    constexpr uint8_t FEUX_RECUL    = 3;   ///< GPIO3 = RX, LED actives à l'état bas
    constexpr uint8_t LED_ETAT      = D4;  ///< GPIO2, LED bleue interne (active à l'état bas)
  }

  /** @brief Liaison série (émission seule : RX sert aux feux de recul) */
  namespace serie
  {
    constexpr uint32_t VITESSE_BAUD = 115200;
  }

  /** @brief Éclairage et LED d'état */
  namespace eclairage
  {
    constexpr bool PHARES_ACTIF_BAS     = true;
    constexpr bool FEUX_RECUL_ACTIF_BAS = true;
    constexpr bool LED_ETAT_ACTIF_BAS   = true;
    constexpr uint32_t PERIODE_ATTENTE_MS = 1000;  ///< clignotement lent : aucun conducteur
    constexpr uint32_t PERIODE_ERREUR_MS  = 150;   ///< clignotement rapide : erreur d'initialisation
  }

  /** @brief Buzzer : bip de recul et klaxon */
  namespace son
  {
    constexpr uint16_t FREQUENCE_BIP_HZ    = 2000;
    constexpr uint16_t DUREE_BIP_MS        = 60;   ///< durée d'un bip du mode périodique
    constexpr uint16_t FREQUENCE_KLAXON_HZ = 440;
    constexpr uint16_t DUREE_KLAXON_MS     = 400;
  }

  /** @brief Servo de direction (à calibrer avec l'exemple TestServo) */
  namespace direction
  {
    constexpr uint8_t ANGLE_NEUTRE      = 90;    ///< degrés, roues droites
    constexpr uint8_t AMPLITUDE         = 35;    ///< degrés de part et d'autre du neutre
    constexpr uint16_t IMPULSION_MIN_US = 1000;  ///< impulsion pour 0°
    constexpr uint16_t IMPULSION_MAX_US = 2000;  ///< impulsion pour 180°
  }

  /** @brief Capteur ultrason arrière */
  namespace ultrason
  {
    constexpr uint32_t PERIODE_MESURE_MS = 60;   ///< minimum conseillé par le fabricant
    constexpr uint16_t PORTEE_MAX_CM     = 300;  ///< au-delà : « pas d'obstacle »
    constexpr uint8_t ECHECS_MAX         = 5;    ///< échecs consécutifs avant ErreurMesure
  }

  /** @brief Propulsion (BTS7960) */
  namespace moteur
  {
    constexpr uint32_t FREQUENCE_PWM_HZ    = 20000;  ///< inaudible, < 25 kHz max du BTS7960
    constexpr uint16_t PENTE_ACCELERATION  = 200;    ///< %/s : 0 → 100 % en 0,5 s
    constexpr uint16_t PENTE_DECELERATION  = 400;    ///< %/s : freinage plus vif
    constexpr uint16_t PAUSE_INVERSION_MS  = 300;    ///< arrêt imposé avant changement de sens
  }

  /** @brief Aide au recul */
  namespace recul
  {
    constexpr uint16_t SEUIL_ARRET_DEFAUT_CM  = 20;
    constexpr uint16_t SEUIL_ARRET_MIN_CM     = 10;
    constexpr uint16_t SEUIL_ARRET_MAX_CM     = 100;
    constexpr uint16_t HYSTERESIS_CM          = 5;   ///< déblocage à seuil + hystérésis
    constexpr uint16_t DISTANCE_LIMITATION_CM = 60;  ///< en dessous, recul plafonné
    constexpr int8_t   VITESSE_RECUL_PROCHE   = 30;  ///< % maximum sous DISTANCE_LIMITATION_CM

    /** @brief Palier du bip de recul : jusqu'à distanceMaxCm, un bip toutes les periodeMs */
    struct PalierBip
    {
      uint16_t distanceMaxCm;
      uint16_t periodeMs;
    };
    /** @brief Paliers du bip (au-delà du dernier : silence) */
    constexpr PalierBip PALIERS_BIP[] = {
      {30, 120},
      {50, 250},
      {80, 500},
      {120, 800},
    };

    static_assert(SEUIL_ARRET_MIN_CM <= SEUIL_ARRET_DEFAUT_CM
                  && SEUIL_ARRET_DEFAUT_CM <= SEUIL_ARRET_MAX_CM,
                  "seuil d'arret par defaut hors bornes");
  }

  /** @brief Sécurités */
  namespace securite
  {
    constexpr uint32_t TIMEOUT_COMMANDE_MS = 500;  ///< sans commande : arrêt
  }

  /** @brief Messages échangés avec la page (voir docs/protocole.md) */
  namespace protocole
  {
    constexpr int POURCENT_MAX     = 100;  ///< vitesse et direction de -100 à 100 %
    constexpr size_t LONGUEUR_MAX  = 32;   ///< au-delà, le message est refusé
  }

  /** @brief Point d'accès Wi-Fi et serveurs */
  namespace reseau
  {
    constexpr char SSID[]         = "VoitureRC";
    constexpr char MOT_DE_PASSE[] = "insa-voiture";  ///< WPA2 : 8 caractères minimum
    constexpr uint8_t CANAL       = 6;
    constexpr uint8_t CLIENTS_MAX = 1;               ///< un seul conducteur
    constexpr uint16_t PORT_HTTP      = 80;
    constexpr uint16_t PORT_WEBSOCKET = 81;
    constexpr uint32_t PERIODE_ETAT_MS     = 200;   ///< diffusion de l'état vers la page
    constexpr uint32_t PERIODE_COMMANDE_MS = 100;   ///< envoi des commandes par la page

    static_assert(sizeof(MOT_DE_PASSE) - 1 >= 8, "WPA2 exige au moins 8 caracteres");
    static_assert(PERIODE_COMMANDE_MS * 3 <= securite::TIMEOUT_COMMANDE_MS,
                  "le chien de garde doit tolerer plusieurs trames perdues");
  }
}

#endif
