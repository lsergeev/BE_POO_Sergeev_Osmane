# Protocole page web ↔ ESP8266

## 1. Transport :

- HTTP (port 80) : juste pour charger la page.
- WebSocket (port 81, `ws://192.168.4.1:81/`) : commandes et état, en texte.
- Un seul conducteur à la fois, le deuxième est refusé.

## 2. Page → ESP :

| Message | Sens | Quand |
|---|---|---|
| `C v d` | vitesse et direction, de −100 à 100 % | 10 fois par seconde, même `C 0 0` (sert de battement de cœur) |
| `P 0` / `P 1` | phares | à l'appui |
| `K` | klaxon (400 ms) | à l'appui |
| `S n` | seuil d'arrêt en recul, de 10 à 100 cm | au relâchement du curseur |

Un message faux (lettre inconnue, nombre manquant, hors bornes) lève une `ErreurProtocole`. Il est ignoré et l'ESP répond `{"t":"err","msg":"..."}`. Rien ne bouge.

## 3. ESP → page :

Du JSON, parce que le navigateur le lit directement. On l'écrit avec `snprintf`, sans bibliothèque. Envoyé toutes les 200 ms.

```json
{"t":"etat","v":-35,"c":-50,"dir":0,"dist":42,"seuil":20,"ph":1,"fr":1,"capt":1,"mode":"ARRIERE"}
```

- `v` : vitesse appliquée ; `c` : consigne ; `dir` : direction (en %).
- `dist` : distance en cm (`-1` si le capteur est KO) ; `seuil` : seuil d'arrêt en cm.
- `ph`, `fr`, `capt` : phares, feux de recul, capteur OK (0 ou 1).
- `mode` : `ARRET`, `AVANT`, `ARRIERE`, `BLOQUE` ou `SECURITE`.

## 4. Temps (dans `Config.h`) :

- Commandes : toutes les 100 ms.
- Chien de garde : 500 ms sans commande, arrêt.
- État : toutes les 200 ms.
- Ultrason : une mesure toutes les 60 ms.

Si on verrouille le téléphone, la page envoie `C 0 0` et se déconnecte. De toute façon, c'est l'ESP qui décide de la sécurité.
