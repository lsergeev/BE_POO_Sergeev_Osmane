/*********************************************************************
 * @file  Protocole.cpp
 * @author L. Sergeev, J. Osmane
 * @brief Décodage des commandes de la page web
 *********************************************************************/
#include "Protocole.h"
#include "Config.h"
#include <stdlib.h>
#include <string.h>

namespace
{
  /** @brief Vrai pour les caractères qu'on tolère autour des valeurs */
  bool estBlanc(char c)
  {
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
  }
}

bool Commande::operator==(const Commande& autre) const
{
  return type == autre.type && vitesse == autre.vitesse
         && direction == autre.direction && valeur == autre.valeur;
}

Commande Protocole::decoder(const char* texte)
{
  using namespace config;
  if (texte == nullptr || texte[0] == '\0')
  {
    throw ErreurProtocole("message vide");
  }
  if (strnlen(texte, protocole::LONGUEUR_MAX + 1) > protocole::LONGUEUR_MAX)
  {
    throw ErreurProtocole("message trop long");
  }

  const char* curseur = texte + 1;
  Commande commande;
  switch (texte[0])
  {
    case 'C':
      commande.type = TypeCommande::CONDUITE;
      commande.vitesse = static_cast<int8_t>(
        lireEntier(curseur, -protocole::POURCENT_MAX, protocole::POURCENT_MAX, "vitesse"));
      commande.direction = static_cast<int8_t>(
        lireEntier(curseur, -protocole::POURCENT_MAX, protocole::POURCENT_MAX, "direction"));
      break;
    case 'P':
      commande.type = TypeCommande::PHARES;
      commande.valeur = static_cast<uint16_t>(lireEntier(curseur, 0, 1, "phares"));
      break;
    case 'K':
      commande.type = TypeCommande::KLAXON;
      break;
    case 'S':
      commande.type = TypeCommande::SEUIL;
      commande.valeur = static_cast<uint16_t>(
        lireEntier(curseur, recul::SEUIL_ARRET_MIN_CM, recul::SEUIL_ARRET_MAX_CM, "seuil"));
      break;
    default:
      throw ErreurProtocole(std::string("commande inconnue '") + texte[0] + "'");
  }
  exigerFin(curseur);
  return commande;
}

size_t Protocole::encoder(const EtatVoiture& etat, char* tampon, size_t taille)
{
  const int n = snprintf(tampon, taille,
    "{\"t\":\"etat\",\"v\":%d,\"c\":%d,\"dir\":%d,\"dist\":%d,\"seuil\":%u,"
    "\"ph\":%d,\"fr\":%d,\"capt\":%d,\"mode\":\"%s\"}",
    etat.vitesse, etat.consigne, etat.direction, etat.distanceCm,
    static_cast<unsigned>(etat.seuilCm), etat.phares ? 1 : 0, etat.feuxRecul ? 1 : 0,
    etat.capteurOk ? 1 : 0, nomMode(etat.mode));
  if (n < 0 || static_cast<size_t>(n) >= taille)
  {
    throw ErreurProtocole("tampon trop petit pour l'etat");
  }
  return static_cast<size_t>(n);
}

size_t Protocole::encoderErreur(const char* raison, char* tampon, size_t taille)
{
  constexpr char DEBUT[] = "{\"t\":\"err\",\"msg\":\"";
  constexpr char FIN[] = "\"}";
  const size_t longueurFixe = sizeof(DEBUT) - 1 + sizeof(FIN) - 1;
  if (taille <= longueurFixe)
  {
    return 0;
  }
  size_t i = 0;
  for (const char* c = DEBUT; *c != '\0'; ++c)
  {
    tampon[i++] = *c;
  }
  // On recopie la raison en évitant de casser le JSON (guillemets, antislash).
  for (const char* c = raison; c != nullptr && *c != '\0' && i + sizeof(FIN) < taille; ++c)
  {
    tampon[i++] = (*c == '"' || *c == '\\') ? '\'' : *c;
  }
  for (const char* c = FIN; *c != '\0'; ++c)
  {
    tampon[i++] = *c;
  }
  tampon[i] = '\0';
  return i;
}

const char* Protocole::nomMode(ModeConduite mode)
{
  switch (mode)
  {
    case ModeConduite::ARRET:    return "ARRET";
    case ModeConduite::AVANT:    return "AVANT";
    case ModeConduite::ARRIERE:  return "ARRIERE";
    case ModeConduite::BLOQUE:   return "BLOQUE";
    case ModeConduite::SECURITE: return "SECURITE";
  }
  return "?";
}

long Protocole::lireEntier(const char*& curseur, long min, long max, const char* champ)
{
  if (*curseur != ' ')
  {
    throw ErreurProtocole(std::string(champ) + " manquant");
  }
  while (*curseur == ' ')
  {
    ++curseur;
  }
  char* fin = nullptr;
  const long valeur = strtol(curseur, &fin, 10);
  if (fin == curseur)
  {
    throw ErreurProtocole(std::string(champ) + " n'est pas un nombre");
  }
  if (valeur < min || valeur > max)
  {
    throw ErreurProtocole(std::string(champ) + " hors bornes");
  }
  curseur = fin;
  return valeur;
}

void Protocole::exigerFin(const char* curseur)
{
  while (estBlanc(*curseur))
  {
    ++curseur;
  }
  if (*curseur != '\0')
  {
    throw ErreurProtocole("caracteres en trop");
  }
}
