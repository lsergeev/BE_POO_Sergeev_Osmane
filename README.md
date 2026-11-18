# projet_voiture_sergeev_osmane

Voiture RC pilotée depuis le navigateur d'un téléphone, avec une page web servie par un ESP8266. BE C++ de l'INSA Toulouse, 4AE FISA.
OSMANE Johan, SERGEEV Leonid.

En plus de la conduite : aide au recul (feux, bip qui accélère, arrêt automatique sous un seuil réglable).

> Séance 1 : la bibliothèque contient la LED, le buzzer, le servo et l'ultrason. Le moteur et la page arrivent en séance 2.
> Conception : [docs/architecture.md](docs/architecture.md).

## 1. Matériel :

- NodeMCU 1.0 (ESP-12E) + shield Grove
- Moteur CC 30 W + BTS7960
- Servo, ultrason et buzzer Grove
- 4 LEDs (2 phares, 2 feux de recul)
- Batterie moteur, convertisseur 5 V, fusible

## 2. Brochage :

| D1 | D2 | D8 | D5 | D6 | D7 | D3 | RX | D4 |
|---|---|---|---|---|---|---|---|---|
| RPWM | LPWM | EN | servo | ultrason | buzzer | phares | feux de recul | LED d'état |

Détails : [docs/cablage.md](docs/cablage.md).

## 3. Installation :

1. IDE Arduino 2.
2. Ajouter dans *Préférences > URL de gestionnaire de cartes* : `https://arduino.esp8266.com/stable/package_esp8266com_index.json`, puis installer **esp8266 3.1.2**.
3. Carte : **NodeMCU 1.0 (ESP-12E Module)**.
4. **Outils > C++ Exceptions > Enabled** (obligatoire, sinon ça ne compile pas).
5. Lier la bibliothèque du projet :
   - Windows : `mklink /J "%USERPROFILE%\Documents\Arduino\libraries\PeripheriquesIoT" "<dépôt>\libraries\PeripheriquesIoT"`
   - Linux : `ln -s "<dépôt>/libraries/PeripheriquesIoT" ~/Arduino/libraries/`

Bibliothèques : core esp8266 3.1.2, PeripheriquesIoT 0.1.0 (ce dépôt), WebSockets de Markus Sattler (version notée en séance 2).

## 4. Compiler et téléverser :

- IDE : ouvrir `projet_voiture_sergeev_osmane/projet_voiture_sergeev_osmane.ino` et téléverser.
- Tests : *Fichier > Exemples > PeripheriquesIoT*.
- Ligne de commande (compile tout, échoue au moindre warning) :
  ```bash
  outils/compiler.sh
  ```
  ou `powershell -ExecutionPolicy Bypass -File outils\compiler.ps1`.
- Moniteur série à 115200 bauds.

## 5. Utilisation :

À compléter en séance 2 : Wi-Fi `VoitureRC`, page `http://192.168.4.1`. Protocole : [docs/protocole.md](docs/protocole.md).

## 6. Archive :

```bash
git archive --format=zip --prefix=projet_voiture_sergeev_osmane/ -o projet_voiture_sergeev_osmane_sources.zip HEAD
```
