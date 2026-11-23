/*********************************************************************
 * @file  Servomoteur.h
 * @author L. Sergeev, J. Osmane
 * @brief Servomoteur de direction commandé en pourcentage autour d'un neutre
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_SERVOMOTEUR_H_
#define PERIPHERIQUES_IOT_SERVOMOTEUR_H_

#include <Servo.h>
#include "Actionneur.h"

/**
 * @class Servomoteur
 * @brief Servo de direction : orienter(-100..100) autour d'un angle neutre
 */
class Servomoteur : public Actionneur
{
  public:
    static constexpr int POURCENT_MAX = 100;  ///< braquage maximal (en %)
    static constexpr uint8_t ANGLE_MAX = 180; ///< course d'un servo standard (degrés)

    /**
     * @brief Constructeur
     * @param nom nom lisible
     * @param broche broche GPIO (PWM nécessaire)
     * @param angleNeutre angle des roues droites (degrés)
     * @param amplitude braquage maximal de part et d'autre du neutre (degrés)
     * @param impulsionMinUs largeur d'impulsion pour 0°
     * @param impulsionMaxUs largeur d'impulsion pour 180°
     */
    Servomoteur(const char* nom, uint8_t broche, uint8_t angleNeutre, uint8_t amplitude,
                uint16_t impulsionMinUs, uint16_t impulsionMaxUs);

    /**
     * @brief Oriente le servo
     * @param pourcent de -100 (gauche toute) à +100 (droite toute), borné
     */
    void orienter(int pourcent);

    /** @brief Orientation courante en % */
    int8_t orientation() const { return orientation_; }
    /** @brief Angle courant en degrés */
    uint8_t angle() const;

    /** @brief État sûr : retour au neutre */
    void arreter() override;
    /** @brief « Servomoteur » */
    const char* type() const override { return "Servomoteur"; }

  protected:
    void verifierConfiguration() const override;
    void initialiser() override;
    void decrireDetails(Print& sortie) const override;

  private:
    Servo servo_;              ///< pilote du core ESP8266
    uint8_t angleNeutre_;      ///< degrés
    uint8_t amplitude_;        ///< degrés
    uint16_t impulsionMinUs_;  ///< µs pour 0°
    uint16_t impulsionMaxUs_;  ///< µs pour 180°
    int8_t orientation_;       ///< % courant
};

#endif
