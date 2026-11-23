/*********************************************************************
 * @file  Led.h
 * @author L. Sergeev, J. Osmane
 * @brief LED (ou groupe de LED sur une même broche) tout-ou-rien
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_LED_H_
#define PERIPHERIQUES_IOT_LED_H_

#include "Actionneur.h"

/**
 * @class Led
 * @brief LED pilotée par une broche, avec clignotement non bloquant
 */
class Led : public Actionneur
{
  public:
    /**
     * @brief Constructeur
     * @param nom nom lisible
     * @param broche broche GPIO
     * @param actifBas vrai si la LED s'allume quand la broche est à 0
     */
    Led(const char* nom, uint8_t broche, bool actifBas = false);

    /** @brief Allume (et arrête un éventuel clignotement) */
    void allumer();
    /** @brief Éteint (et arrête un éventuel clignotement) */
    void eteindre();
    /** @brief Allume ou éteint selon le paramètre */
    void commuter(bool allumee);
    /** @brief Inverse l'état courant */
    void basculer();
    /**
     * @brief Fait clignoter la LED (rapport cyclique 50 %)
     * @param periodeMs période complète (allumée + éteinte), en ms
     */
    void clignoter(uint32_t periodeMs);

    /** @brief Vrai si la LED est allumée en ce moment */
    bool estAllumee() const { return allumee_; }
    /** @brief Vrai si un clignotement est en cours */
    bool clignote() const { return periodeClignotementMs_ != 0; }

    /** @brief État sûr : éteinte */
    void arreter() override;
    /** @brief Gère le clignotement */
    void update(uint32_t maintenantMs) override;
    /** @brief « Led » */
    const char* type() const override { return "Led"; }

  protected:
    void initialiser() override;
    void decrireDetails(Print& sortie) const override;

  private:
    /** @brief Écrit l'état sur la broche (si initialisée) */
    void ecrire(bool allumee);

    bool actifBas_;                    ///< logique inversée
    bool allumee_;                     ///< état courant
    uint32_t periodeClignotementMs_;   ///< 0 : pas de clignotement
    uint32_t dernierBasculementMs_;    ///< date du dernier changement d'état
};

#endif
