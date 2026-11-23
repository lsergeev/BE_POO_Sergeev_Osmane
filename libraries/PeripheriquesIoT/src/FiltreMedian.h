/*********************************************************************
 * @file  FiltreMedian.h
 * @author L. Sergeev, J. Osmane
 * @brief Filtre médian glissant générique (modèle, entièrement dans l'en-tête)
 *********************************************************************/
#ifndef PERIPHERIQUES_IOT_FILTRE_MEDIAN_H_
#define PERIPHERIQUES_IOT_FILTRE_MEDIAN_H_

#include <array>
#include <algorithm>
#include <stdexcept>
#include <stddef.h>

/**
 * @class FiltreMedian
 * @brief Médiane des N dernières valeurs
 * @tparam T type des valeurs (comparable avec <)
 * @tparam N nombre de valeurs conservées
 */
template <typename T, size_t N>
class FiltreMedian
{
    static_assert(N > 0, "FiltreMedian : N doit etre strictement positif");

  public:
    /**
     * @brief Ajoute une valeur (remplace la plus ancienne si le filtre est plein)
     * @param valeur nouvelle valeur
     */
    void ajouter(T valeur)
    {
      valeurs_[prochain_] = valeur;
      prochain_ = (prochain_ + 1) % N;
      if (nb_ < N)
      {
        ++nb_;
      }
    }

    /**
     * @brief Médiane des valeurs présentes
     * @throw std::out_of_range si le filtre est vide
     */
    T mediane() const
    {
      if (nb_ == 0)
      {
        throw std::out_of_range("FiltreMedian : aucune valeur");
      }
      // Tant que le filtre n'est pas plein, les valeurs occupent les nb_ premières cases.
      std::array<T, N> copie = valeurs_;
      const auto fin = copie.begin() + nb_;
      const auto milieu = copie.begin() + nb_ / 2;
      std::nth_element(copie.begin(), milieu, fin);
      return *milieu;
    }

    /** @brief Oublie toutes les valeurs */
    void vider() { nb_ = 0; prochain_ = 0; }
    /** @brief Nombre de valeurs présentes */
    size_t taille() const { return nb_; }
    /** @brief Vrai si aucune valeur */
    bool estVide() const { return nb_ == 0; }

  private:
    std::array<T, N> valeurs_{};  ///< tampon circulaire
    size_t prochain_ = 0;         ///< case de la prochaine écriture
    size_t nb_ = 0;               ///< nombre de valeurs valides
};

#endif
