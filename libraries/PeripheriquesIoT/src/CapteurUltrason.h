/*********************************************************************
 * @file  CapteurUltrason.h
 * @author L. Sergeev, J. Osmane
 * @brief Télémètre à ultrasons à broche unique (Grove Ultrasonic Ranger)
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_CAPTEUR_ULTRASON_H_
#define PERIPHERIQUES_IOT_CAPTEUR_ULTRASON_H_

#include "Capteur.h"
#include "FiltreMedian.h"

/**
 * @class CapteurUltrason
 * @brief Mesure de distance non bloquante, par interruption
 */
class CapteurUltrason : public Capteur
{
  public:
    static constexpr size_t TAILLE_FILTRE = 5;  ///< valeurs pour la médiane

    /**
     * @brief Constructeur
     * @param nom nom lisible
     * @param broche broche SIG (déclenchement et écho), interruption nécessaire
     * @param periodeMs période de mesure (60 ms minimum conseillé)
     * @param porteeMaxCm distance retournée quand l'obstacle est hors de portée
     * @param echecsMax échecs consécutifs tolérés avant de déclarer le capteur en défaut
     */
    CapteurUltrason(const char* nom, uint8_t broche, uint32_t periodeMs,
                    uint16_t porteeMaxCm, uint8_t echecsMax);

    /** @brief Détache l'interruption */
    ~CapteurUltrason() override;

    /**
     * @brief Distance filtrée (médiane des dernières mesures)
     * @return distance en cm, au plus porteeMaxCm
     * @throw ErreurMesure si le capteur n'est pas fiable (absent, débranché, pas encore de mesure)
     */
    uint16_t distanceCm() const;

    /** @brief Vrai si au moins une mesure valide existe et que les échecs restent sous le seuil */
    bool estFiable() const;
    /** @brief Nombre d'échecs consécutifs */
    uint8_t echecsConsecutifs() const { return echecs_; }
    /** @brief « CapteurUltrason » */
    const char* type() const override { return "CapteurUltrason"; }

  protected:
    void verifierConfiguration() const override;
    void initialiser() override;
    void acquerir(uint32_t maintenantMs) override;
    void decrireDetails(Print& sortie) const override;

  private:
    /** @brief Routine d'interruption (fronts montant et descendant de l'écho) */
    static void IRAM_ATTR isrEcho(void* arg);
    /** @brief Envoie l'impulsion de déclenchement et arme l'interruption */
    void declencher();
    /** @brief Exploite l'écho de la mesure précédente */
    void exploiterEcho();
    /** @brief Comptabilise un échec */
    void signalerEchec();

    FiltreMedian<uint16_t, TAILLE_FILTRE> filtre_;  ///< médiane des mesures
    uint16_t porteeMaxCm_;                          ///< saturation de la distance
    uint8_t echecsMax_;                             ///< seuil de défaut
    uint8_t echecs_;                                ///< échecs consécutifs
    bool mesureEnCours_;                            ///< une impulsion a été envoyée
    volatile uint32_t debutEchoUs_;                 ///< front montant (ISR)
    volatile uint32_t finEchoUs_;                   ///< front descendant (ISR)
    volatile bool frontMontant_;                    ///< front montant vu (ISR)
    volatile bool echoTermine_;                     ///< front descendant vu (ISR)
};

#endif
