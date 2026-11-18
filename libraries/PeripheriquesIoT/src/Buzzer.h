/*********************************************************************
 * @file  Buzzer.h
 * @author L. Sergeev, J. Osmane
 * @brief Buzzer piézo : bips ponctuels, bips périodiques, son continu
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_BUZZER_H_
#define PERIPHERIQUES_IOT_BUZZER_H_

#include "Actionneur.h"

/**
 * @class Buzzer
 * @brief Buzzer piloté par tone()/noTone(), entièrement non bloquant
 *
 * Deux couches de son :
 *  - un « fond » permanent : silence, bips périodiques ou son continu ;
 *  - un bip ponctuel (ex. klaxon) prioritaire : il masque le fond
 *    pendant sa durée, puis le fond reprend.
 * update() doit être appelée à chaque tour de boucle.
 */
class Buzzer : public Actionneur
{
  public:
    /**
     * @brief Constructeur
     * @param nom nom lisible
     * @param broche broche GPIO
     * @param frequenceHz fréquence par défaut
     * @param dureeBipMs durée d'un bip en mode périodique
     */
    Buzzer(const char* nom, uint8_t broche, uint16_t frequenceHz, uint16_t dureeBipMs);

    /**
     * @brief Bip ponctuel prioritaire, à la fréquence par défaut
     * @param dureeMs durée du bip
     */
    void bip(uint16_t dureeMs);
    /**
     * @brief Bip ponctuel prioritaire
     * @param dureeMs durée du bip
     * @param frequenceHz fréquence du bip
     */
    void bip(uint16_t dureeMs, uint16_t frequenceHz);
    /**
     * @brief Fond : un bip toutes les periodeMs
     *
     * Rappeler avec la même période ne relance pas le cycle : la méthode
     * peut être appelée à chaque tour de boucle.
     * @param periodeMs période entre deux débuts de bip
     */
    void bipPeriodique(uint16_t periodeMs);
    /** @brief Fond : son continu */
    void continu();
    /** @brief Fond : silence (un bip ponctuel en cours se termine normalement) */
    void silence();

    /** @brief Vrai si le buzzer émet en ce moment */
    bool sonne() const { return sonActif_; }

    /** @brief État sûr : silence complet immédiat */
    void arreter() override;
    /** @brief Calcule et applique le son à émettre */
    void update(uint32_t maintenantMs) override;
    /** @brief « Buzzer » */
    const char* type() const override { return "Buzzer"; }

  protected:
    void verifierConfiguration() const override;
    void initialiser() override;
    void decrireDetails(Print& sortie) const override;

  private:
    /** @brief Son de fond */
    enum class Fond : uint8_t { SILENCE, PERIODIQUE, CONTINU };

    /** @brief Commande tone()/noTone() seulement en cas de changement */
    void appliquer(bool actif, uint16_t frequenceHz);

    uint16_t frequenceHz_;       ///< fréquence par défaut
    uint16_t dureeBipMs_;        ///< durée d'un bip périodique
    Fond fond_;                  ///< son de fond
    uint16_t periodeMs_;         ///< période du fond PERIODIQUE
    uint32_t debutCycleMs_;      ///< origine du cycle périodique
    bool bipPonctuel_;           ///< bip prioritaire en cours
    uint32_t finBipMs_;          ///< fin du bip prioritaire
    uint16_t frequenceBipHz_;    ///< fréquence du bip prioritaire
    bool sonActif_;              ///< son réellement émis
    uint16_t frequenceActiveHz_; ///< fréquence réellement émise
};

#endif
