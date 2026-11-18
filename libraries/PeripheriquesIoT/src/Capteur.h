/*********************************************************************
 * @file  Capteur.h
 * @author L. Sergeev, J. Osmane
 * @brief Classe de base abstraite des capteurs à acquisition périodique
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_CAPTEUR_H_
#define PERIPHERIQUES_IOT_CAPTEUR_H_

#include "Peripherique.h"

/**
 * @class Capteur
 * @brief Périphérique qui mesure à intervalle régulier, sans bloquer
 *
 * update() se charge du cadencement avec millis() et appelle acquerir()
 * toutes les periodeMs. Les classes filles n'écrivent que acquerir().
 */
class Capteur : public Peripherique
{
  public:
    /**
     * @brief Constructeur
     * @param nom nom lisible
     * @param broche broche GPIO principale
     * @param periodeMs intervalle entre deux acquisitions (ms)
     */
    Capteur(const char* nom, uint8_t broche, uint32_t periodeMs);

    /**
     * @brief Déclenche acquerir() quand la période est écoulée
     * @param maintenantMs valeur courante de millis()
     */
    void update(uint32_t maintenantMs) override;

    /** @brief Intervalle entre deux acquisitions (ms) */
    uint32_t periodeMs() const { return periodeMs_; }

  protected:
    /**
     * @brief Réalise une acquisition (ne doit pas bloquer)
     * @param maintenantMs valeur courante de millis()
     */
    virtual void acquerir(uint32_t maintenantMs) = 0;

  private:
    uint32_t periodeMs_;              ///< période d'acquisition
    uint32_t derniereAcquisitionMs_;  ///< date de la dernière acquisition
    bool premiereAcquisition_;        ///< vrai tant qu'aucune acquisition n'a eu lieu
};

#endif
