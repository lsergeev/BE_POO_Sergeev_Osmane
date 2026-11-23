/*********************************************************************
 * @file  Protocole.h
 * @author L. Sergeev, J. Osmane
 * @brief Messages échangés entre la page web et l'ESP (docs/protocole.md)
 *********************************************************************/
#ifndef PROTOCOLE_H_
#define PROTOCOLE_H_

#include <Arduino.h>
#include <stdexcept>
#include <string>

/**
 * @class ErreurProtocole
 * @brief Message reçu invalide : il est ignoré, la voiture ne bouge pas
 */
class ErreurProtocole : public std::runtime_error
{
  public:
    /**
     * @brief Construit l'erreur
     * @param message raison du refus
     */
    explicit ErreurProtocole(const std::string& message)
      : std::runtime_error(message) {}
};

/** @brief Les différentes commandes que la page peut envoyer */
enum class TypeCommande : uint8_t
{
  CONDUITE,  ///< « C v d » : vitesse et direction
  PHARES,    ///< « P 0|1 »
  KLAXON,    ///< « K »
  SEUIL      ///< « S n » : seuil d'arrêt en recul (cm)
};

/**
 * @struct Commande
 * @brief Une commande décodée
 */
struct Commande
{
  TypeCommande type = TypeCommande::CONDUITE;  ///< type de commande
  int8_t vitesse = 0;                          ///< CONDUITE : -100 à 100 %
  int8_t direction = 0;                        ///< CONDUITE : -100 à 100 %
  uint16_t valeur = 0;                         ///< PHARES (0/1) ou SEUIL (cm)

  /** @brief Deux commandes identiques (sert à ne journaliser que les changements) */
  bool operator==(const Commande& autre) const;
  /** @brief Contraire de == */
  bool operator!=(const Commande& autre) const { return !(*this == autre); }
};

/** @brief Mode de conduite affiché sur la page */
enum class ModeConduite : uint8_t
{
  ARRET,     ///< vitesse nulle
  AVANT,     ///< marche avant
  ARRIERE,   ///< marche arrière
  BLOQUE,    ///< recul bloqué par un obstacle
  SECURITE   ///< liaison perdue ou erreur : moteur arrêté
};

/**
 * @struct EtatVoiture
 * @brief Ce que l'ESP renvoie à la page 5 fois par seconde
 */
struct EtatVoiture
{
  int8_t vitesse = 0;                       ///< vitesse appliquée (%)
  int8_t consigne = 0;                      ///< consigne reçue (%)
  int8_t direction = 0;                     ///< direction (%)
  int16_t distanceCm = -1;                  ///< distance arrière, -1 si capteur KO
  uint16_t seuilCm = 0;                     ///< seuil d'arrêt en recul
  bool phares = false;                      ///< phares allumés
  bool feuxRecul = false;                   ///< feux de recul allumés
  bool capteurOk = false;                   ///< capteur arrière fiable
  ModeConduite mode = ModeConduite::ARRET;  ///< mode courant
};

/**
 * @class Protocole
 * @brief Décodage des commandes (texte → Commande) et encodage de l'état (→ JSON)
 *
 * Classe utilitaire : que des méthodes statiques, pas d'instance.
 */
class Protocole
{
  public:
    static constexpr size_t TAILLE_TAMPON = 160;  ///< taille conseillée pour encoder()

    Protocole() = delete;

    /**
     * @brief Écrit l'état en JSON, par ex. {"t":"etat","v":-35,...}
     * @param etat état de la voiture
     * @param tampon zone où écrire
     * @param taille taille du tampon
     * @return nombre de caractères écrits (sans le '\0')
     * @throw ErreurProtocole si le tampon est trop petit
     */
    static size_t encoder(const EtatVoiture& etat, char* tampon, size_t taille);

    /**
     * @brief Écrit un message d'erreur en JSON : {"t":"err","msg":"..."}
     * @param raison texte de l'erreur (les guillemets sont remplacés)
     * @param tampon zone où écrire
     * @param taille taille du tampon
     * @return nombre de caractères écrits, 0 si le tampon est trop petit
     */
    static size_t encoderErreur(const char* raison, char* tampon, size_t taille);

    /** @brief Nom du mode tel qu'envoyé à la page (« ARRET », « AVANT », ...) */
    static const char* nomMode(ModeConduite mode);

    /**
     * @brief Décode un message texte reçu par WebSocket
     * @param texte message terminé par '\0'
     * @return la commande correspondante
     * @throw ErreurProtocole message vide, trop long, inconnu ou valeur hors bornes
     */
    static Commande decoder(const char* texte);

  private:
    /**
     * @brief Lit un entier précédé d'au moins un espace et avance le curseur
     * @throw ErreurProtocole nombre absent ou hors bornes
     */
    static long lireEntier(const char*& curseur, long min, long max, const char* champ);
    /** @brief Vérifie qu'il ne reste que des espaces ou des fins de ligne */
    static void exigerFin(const char* curseur);
};

#endif
