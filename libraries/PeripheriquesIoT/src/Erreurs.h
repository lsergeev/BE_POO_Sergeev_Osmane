/*********************************************************************
 * @file  Erreurs.h
 * @author L. Sergeev, J. Osmane
 * @brief Exceptions levées par la bibliothèque PeripheriquesIoT
 *
 * Toutes dérivent de std::runtime_error : l'application peut les
 * attraper finement (ErreurBroche, ErreurMesure) ou globalement
 * (ErreurPeripherique, std::exception).
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_ERREURS_H_
#define PERIPHERIQUES_IOT_ERREURS_H_

#include <stdexcept>
#include <string>
#include <stdint.h>

// Le core ESP8266 compile par défaut avec -fno-exceptions : on préfère
// une erreur de compilation explicite à un programme qui s'arrête au
// premier throw.
#if !defined(__cpp_exceptions) && !defined(__EXCEPTIONS)
#error "Exceptions C++ desactivees : dans l'IDE Arduino, Outils > C++ Exceptions > Enabled (FQBN : ...:exception=enabled)"
#endif

/**
 * @class ErreurPeripherique
 * @brief Erreur générique d'un périphérique (configuration incohérente, etc.)
 */
class ErreurPeripherique : public std::runtime_error
{
  public:
    /**
     * @brief Construit l'erreur
     * @param message description lisible, affichée par what()
     */
    explicit ErreurPeripherique(const std::string& message)
      : std::runtime_error(message) {}
};

/**
 * @class ErreurBroche
 * @brief Broche inexistante, réservée (flash) ou incapable de la fonction demandée (PWM, interruption)
 */
class ErreurBroche : public ErreurPeripherique
{
  public:
    /**
     * @brief Construit l'erreur
     * @param message description lisible
     * @param broche numéro GPIO fautif
     */
    ErreurBroche(const std::string& message, uint8_t broche)
      : ErreurPeripherique(message), broche_(broche) {}

    /** @brief Numéro GPIO à l'origine de l'erreur */
    uint8_t broche() const { return broche_; }

  private:
    uint8_t broche_;
};

/**
 * @class ErreurMesure
 * @brief Capteur incapable de fournir une mesure fiable (absent, débranché, muet)
 */
class ErreurMesure : public ErreurPeripherique
{
  public:
    /**
     * @brief Construit l'erreur
     * @param message description lisible
     */
    explicit ErreurMesure(const std::string& message)
      : ErreurPeripherique(message) {}
};

#endif
