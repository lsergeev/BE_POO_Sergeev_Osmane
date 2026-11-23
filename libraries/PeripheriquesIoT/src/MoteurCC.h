/*********************************************************************
 * @file  MoteurCC.h
 * @author L. Sergeev, J. Osmane
 * @brief Moteur à courant continu : rampe et pause avant inversion
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_MOTEUR_CC_H_
#define PERIPHERIQUES_IOT_MOTEUR_CC_H_

#include "Actionneur.h"

/**
 * @class MoteurCC
 * @brief Partie commune à tous les moteurs CC, quel que soit le pont en H
 *
 * On donne une consigne en % (-100 à 100). update() fait évoluer la
 * vitesse réelle vers la consigne en respectant :
 *  - une pente d'accélération et une pente de décélération (en %/s) ;
 *  - un passage par 0 avant chaque changement de sens, avec une pause
 *    à l'arrêt pour protéger le moteur et le pont.
 * La classe fille n'a qu'à écrire appliquer() pour traduire la vitesse
 * en signaux pour son pont (patron méthode modèle).
 */
class MoteurCC : public Actionneur
{
  public:
    static constexpr int POURCENT_MAX = 100;  ///< vitesse maximale (en %)

    /**
     * @brief Constructeur
     * @param nom nom lisible
     * @param broche broche principale (PWM marche avant)
     * @param penteAcceleration pente quand la vitesse augmente (%/s)
     * @param penteDeceleration pente quand la vitesse diminue (%/s)
     * @param pauseInversionMs temps passé à l'arrêt avant de repartir dans l'autre sens
     */
    MoteurCC(const char* nom, uint8_t broche, uint16_t penteAcceleration,
             uint16_t penteDeceleration, uint16_t pauseInversionMs);

    /**
     * @brief Nouvelle consigne de vitesse
     * @param pourcent de -100 (arrière toute) à 100 (avant toute), borné
     */
    void setConsigne(int pourcent);

    /** @brief Consigne courante en % */
    int8_t consigne() const { return consigne_; }
    /** @brief Vitesse réellement appliquée en % */
    int8_t vitesse() const { return vitesseAppliquee_; }
    /** @brief Vrai si le moteur attend la fin de la pause avant d'inverser */
    bool enPauseInversion() const { return enPause_; }

    /** @brief État sûr : consigne à 0 (le moteur ralentit avec la rampe) */
    void arreter() override;
    /** @brief Arrêt sans rampe (obstacle) */
    void arretImmediat();
    /** @brief Fait avancer la rampe */
    void update(uint32_t maintenantMs) override;

  protected:
    void verifierConfiguration() const override;
    void decrireDetails(Print& sortie) const override;

    /**
     * @brief Envoie la vitesse au pont en H
     * @param vitesse de -100 à 100 (%)
     */
    virtual void appliquer(int8_t vitesse) = 0;

  private:
    /** @brief Signe d'une valeur : -1, 0 ou 1 */
    static int signe(float valeur);
    /** @brief Applique la vitesse si elle a changé */
    void envoyer();

    int8_t consigne_;            ///< consigne en %
    float vitesse_;              ///< vitesse calculée par la rampe
    int8_t vitesseAppliquee_;    ///< dernière vitesse envoyée au pont
    uint16_t penteAcceleration_; ///< %/s
    uint16_t penteDeceleration_; ///< %/s
    uint16_t pauseInversionMs_;  ///< ms à l'arrêt avant de changer de sens
    uint32_t dernierUpdateMs_;   ///< date du dernier update()
    bool premierUpdate_;         ///< pas encore de dernierUpdateMs_ valable
    int dernierSens_;            ///< sens du dernier mouvement (-1, 0 ou 1)
    uint32_t arretDepuisMs_;     ///< date à laquelle la vitesse est tombée à 0
    bool enPause_;               ///< pause d'inversion en cours
};

#endif
