/*********************************************************************
 * @file  MoteurBTS7960.h
 * @author L. Sergeev, J. Osmane
 * @brief Moteur CC piloté par un pont en H BTS7960 (module IBT-2)
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_MOTEUR_BTS7960_H_
#define PERIPHERIQUES_IOT_MOTEUR_BTS7960_H_

#include "MoteurCC.h"

/**
 * @class MoteurBTS7960
 * @brief Traduit la vitesse de MoteurCC en PWM sur RPWM / LPWM
 *
 * Vitesse positive : PWM sur RPWM, LPWM à 0. Négative : l'inverse.
 * Nulle : les deux à 0, ce qui freine le moteur (les deux transistors
 * du bas conduisent). R_EN et L_EN sont reliées à une seule broche :
 * à 0 le pont est coupé (roue libre).
 *
 * Attention : analogWriteFreq() et analogWriteRange() sont globales sur
 * l'ESP8266, elles s'appliquent à toutes les sorties analogWrite().
 */
class MoteurBTS7960 : public MoteurCC
{
  public:
    static constexpr uint16_t PLAGE_PWM = 255;  ///< valeur analogWrite() à 100 %

    /**
     * @brief Constructeur
     * @param nom nom lisible
     * @param brocheRpwm PWM marche avant
     * @param brocheLpwm PWM marche arrière
     * @param brocheEnable R_EN et L_EN reliées
     * @param frequencePwmHz fréquence du PWM (25 kHz max pour le BTS7960)
     * @param penteAcceleration %/s
     * @param penteDeceleration %/s
     * @param pauseInversionMs arrêt imposé avant de changer de sens
     */
    MoteurBTS7960(const char* nom, uint8_t brocheRpwm, uint8_t brocheLpwm, uint8_t brocheEnable,
                  uint32_t frequencePwmHz, uint16_t penteAcceleration,
                  uint16_t penteDeceleration, uint16_t pauseInversionMs);

    /** @brief Coupe le pont (roue libre), même si une consigne arrive ensuite */
    void desactiver();
    /** @brief Réactive le pont après desactiver() */
    void activer();
    /** @brief Vrai si le pont est activé */
    bool estActive() const { return active_; }

    /** @brief « MoteurBTS7960 » */
    const char* type() const override { return "MoteurBTS7960"; }

  protected:
    void verifierConfiguration() const override;
    void initialiser() override;
    void decrireDetails(Print& sortie) const override;
    void appliquer(int8_t vitesse) override;

  private:
    uint8_t brocheLpwm_;       ///< PWM marche arrière
    uint8_t brocheEnable_;     ///< R_EN + L_EN
    uint32_t frequencePwmHz_;  ///< fréquence PWM
    bool active_;              ///< pont activé
};

#endif
