/*********************************************************************
 * @file  Peripherique.h
 * @author L. Sergeev, J. Osmane
 * @brief Classe de base abstraite de tous les capteurs et actionneurs
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_PERIPHERIQUE_H_
#define PERIPHERIQUES_IOT_PERIPHERIQUE_H_

#include <Arduino.h>
#include "Erreurs.h"

/**
 * @class Peripherique
 * @brief Ce qui est commun à tout composant branché sur l'ESP8266
 *
 * Cycle de vie : construction (aucune vérification, aucun accès matériel,
 * donc utilisable pour des objets globaux) → begin() (vérifie la
 * configuration puis initialise le matériel, peut lever une exception)
 * → update() appelée à chaque tour de loop().
 *
 * begin() et decrire() suivent le patron « interface non virtuelle » :
 * le déroulement est fixé ici, les classes filles ne fournissent que
 * les étapes variables (verifierConfiguration(), initialiser(),
 * type(), decrireDetails()).
 *
 * Un périphérique représente une ressource matérielle unique : il n'est
 * pas copiable.
 */
class Peripherique
{
  public:
    /**
     * @brief Constructeur
     * @param nom nom lisible (chaîne littérale, non copiée)
     * @param broche numéro GPIO principal
     */
    Peripherique(const char* nom, uint8_t broche);

    /** @brief Destructeur virtuel : destruction correcte via un pointeur de base */
    virtual ~Peripherique() = default;

    Peripherique(const Peripherique&) = delete;
    Peripherique& operator=(const Peripherique&) = delete;

    /**
     * @brief Vérifie la configuration puis initialise le matériel
     * @throw ErreurBroche broche invalide ou inadaptée
     * @throw ErreurPeripherique autre incohérence de configuration
     */
    void begin();

    /**
     * @brief Traitement périodique non bloquant, à appeler à chaque tour de loop()
     * @param maintenantMs valeur courante de millis()
     */
    virtual void update(uint32_t maintenantMs);

    /**
     * @brief Écrit une description d'une ligne, ex. « [Led 'phares' GPIO0 actif-bas eteinte] »
     * @param sortie flux de sortie (Serial, ...)
     */
    void decrire(Print& sortie) const;

    /** @brief Nom de la classe concrète (« Led », « Buzzer », ...) */
    virtual const char* type() const = 0;

    /** @brief Nom donné au périphérique */
    const char* nom() const { return nom_; }
    /** @brief Broche GPIO principale */
    uint8_t broche() const { return broche_; }
    /** @brief Vrai après un begin() réussi */
    bool estPret() const { return pret_; }

    /** @brief Vrai si la broche existe et n'est pas réservée à la flash (GPIO6 à 11) */
    static bool estBrocheValide(uint8_t broche);
    /** @brief Vrai si la broche peut produire du PWM (toutes les valides sauf GPIO16) */
    static bool estBrochePwm(uint8_t broche);

  protected:
    /**
     * @brief Contrôle la configuration avant tout accès matériel
     *
     * Par défaut, vérifie que la broche principale est valide. Les classes
     * filles complètent (PWM, plages d'angle, ...).
     */
    virtual void verifierConfiguration() const;

    /** @brief Configure le matériel (pinMode, attach, ...). Appelée par begin(). */
    virtual void initialiser() = 0;

    /** @brief Complète la description (état courant). Vide par défaut. */
    virtual void decrireDetails(Print& sortie) const;

    /** @brief Lève ErreurBroche si la broche n'est pas valide */
    void exigerBrocheValide(uint8_t broche) const;
    /** @brief Lève ErreurBroche si la broche ne peut pas faire de PWM */
    void exigerPwm(uint8_t broche) const;

    const char* nom_;   ///< nom lisible
    uint8_t broche_;    ///< broche GPIO principale
    bool pret_;         ///< vrai après begin() réussi
};

/**
 * @brief Écrit la description d'un périphérique : Serial << moteur << servo;
 *
 * Fonction libre qui s'appuie sur decrire(), elle-même construite sur
 * des méthodes virtuelles : elle fonctionne pour toute classe fille,
 * y compris via une référence ou un pointeur de base.
 * @param sortie flux de sortie
 * @param p périphérique à décrire
 * @return le flux, pour enchaîner les <<
 */
Print& operator<<(Print& sortie, const Peripherique& p);

#endif
