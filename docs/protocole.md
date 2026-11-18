# Protocole d'échange page web ↔ ESP8266

## Transport
| Rôle | Canal | Détail |
|---|---|---|
| Livraison de la page | HTTP, port 80 | `GET /` renvoie `PageWeb.h` (HTML, CSS et JS dans un seul fichier, stocké en PROGMEM). Toute autre URL renvoie aussi la page. |
| Pilotage et état | WebSocket, port 81 | `ws://192.168.4.1:81/`, trames **texte**, un message par trame |

On n'accepte qu'un seul conducteur à la fois. Un second client est refusé (fermeture avec un message `{"t":"err","msg":"occupé"}`).

## Page → ESP (commandes)

Ce sont de courts messages ASCII. Le décodage est trivial (`strtol`) et ne demande aucune bibliothèque.

| Message | Paramètres | Fréquence | Effet |
|---|---|---|---|
| `C <v> <d>` | `v` = consigne de vitesse en %, de −100 à 100 (négatif = arrière). `d` = direction en %, de −100 (gauche) à 100 (droite). | **10 Hz en continu** tant que la page est visible, y compris `C 0 0` au repos | Conduite. Sert aussi de **battement de cœur** pour le chien de garde. |
| `P <0\|1>` | 0 = éteint, 1 = allumé | à l'appui | phares |
| `K` | — | à l'appui | coup de klaxon (400 ms, durée fixée côté ESP) |
| `S <cm>` | seuil d'arrêt en recul, de 10 à 100 | au relâchement du curseur | règle le seuil |

Exemples : `C 70 -50` · `C -30 0` · `P 1` · `K` · `S 25`

**Validation** : `Protocole::decoder()` lève une `ErreurProtocole` dans ces cas :
- message vide ;
- lettre inconnue ;
- paramètre manquant ou non numérique ;
- valeur hors bornes.

`ServeurPilotage` attrape l'exception, ignore le message, incrémente un compteur et répond `{"t":"err","msg":"<raison>"}`. Une trame corrompue ne fait donc **jamais** bouger la voiture.

## ESP → page (état)

C'est un objet JSON, parce que le navigateur le lit nativement avec `JSON.parse`. Il est construit avec `snprintf`, sans bibliothèque JSON.
Il est diffusé **toutes les 200 ms (5 Hz)**, ainsi qu'à chaque changement de mode.

```json
{"t":"etat","v":-35,"c":-50,"dir":0,"dist":42,"seuil":20,"ph":1,"fr":1,"capt":1,"mode":"ARRIERE"}
```

| Champ | Type | Signification |
|---|---|---|
| `t` | texte | type de message : `etat` ou `err` |
| `v` | entier, −100 à 100 | vitesse **réellement appliquée** (après rampe et limitation) |
| `c` | entier, −100 à 100 | consigne reçue |
| `dir` | entier, −100 à 100 | direction |
| `dist` | entier | distance arrière en cm, `-1` si le capteur est KO |
| `seuil` | entier | seuil d'arrêt en cm |
| `ph`, `fr` | 0 ou 1 | phares, feux de recul |
| `capt` | 0 ou 1 | capteur arrière fiable |
| `mode` | texte | `ARRET`, `AVANT`, `ARRIERE`, `BLOQUE` ou `SECURITE` |

## Temporisations (toutes dans `Config.h`)

| Constante | Valeur | Rôle |
|---|---|---|
| `PERIODE_COMMANDE_MS` (côté page, répétée dans le JS) | 100 | cadence d'envoi de `C` |
| `TIMEOUT_COMMANDE_MS` | 500 | au-delà, mode SÉCURITÉ et arrêt (5 trames perdues tolérées) |
| `PERIODE_ETAT_MS` | 200 | cadence de diffusion de l'état |
| `PERIODE_MESURE_MS` | 60 | cadence de l'ultrason (minimum recommandé par le fabricant) |

## Comportement de la page
- Elle envoie `C 0 0` puis ferme le WebSocket sur l'événement `visibilitychange` (onglet masqué ou téléphone verrouillé), en plus du chien de garde côté ESP.
- Elle se reconnecte automatiquement toutes les secondes si la liaison est perdue, avec un indicateur rouge ou vert.
- Elle ne s'appuie pas sur une latence garantie : tous les comportements de sécurité sont **décidés côté ESP**.
