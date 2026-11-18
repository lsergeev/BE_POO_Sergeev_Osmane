# projet_voiture_sergeev_osmane — Voiture RC pilotée par téléphone (ESP8266)

BE C++ « Application du C++ au domaine des objets connectés », INSA Toulouse. Binôme : L. Sergeev, J. Osmane.

Une voiture radiocommandée pilotée depuis le navigateur d'un téléphone, grâce à une page web servie par un ESP8266 en point d'accès Wi-Fi. Elle intègre une aide au recul : feux de recul, bip qui accélère à l'approche d'un obstacle et arrêt automatique sous un seuil réglable.

> **État : séance 1.** La bibliothèque contient les composants du kit (LED, buzzer, servo, ultrason). Le moteur, le serveur web et la page arrivent en séance 2.
> La conception est décrite dans [docs/architecture.md](docs/architecture.md).

## Matériel
| Élément | Rôle |
|---|---|
| NodeMCU 1.0 (ESP-12E) et shield Grove pour NodeMCU | microcontrôleur et Wi-Fi |
| Moteur CC 30 W et module BTS7960 (IBT-2) | propulsion avant et arrière |
| Servo Grove | direction |
| Grove Ultrasonic Ranger v2.0 | distance arrière |
| Grove Buzzer | klaxon et bip de recul |
| 4 LED et résistances | 2 phares, 2 feux de recul |
| Batterie moteur, convertisseur 5 V, interrupteur, fusible | alimentation |

## Brochage
| Broche | Fonction | | Broche | Fonction |
|---|---|---|---|---|
| D1 (GPIO5) | BTS7960 RPWM | | D6 (GPIO12) | ultrason SIG |
| D2 (GPIO4) | BTS7960 LPWM | | D7 (GPIO13) | buzzer |
| D8 (GPIO15) | BTS7960 R_EN + L_EN | | D3 (GPIO0) | phares (actifs à l'état bas) |
| D5 (GPIO14) | servo | | RX (GPIO3) | feux de recul (actifs à l'état bas) |
| D4 (GPIO2) | LED d'état interne | | TX | traces série |

Le câblage, l'alimentation et la compatibilité 3,3 V sont détaillés dans [docs/cablage.md](docs/cablage.md).

## Installation
1. **IDE Arduino 2.x.**
2. **Core ESP8266.** Dans *Fichier > Préférences > URL de gestionnaire de cartes supplémentaires*, ajouter :
   `https://arduino.esp8266.com/stable/package_esp8266com_index.json`
   Puis dans *Outils > Carte > Gestionnaire de cartes*, installer **esp8266 by ESP8266 Community, version 3.1.2**.
3. **Carte.** Choisir *Outils > Carte > ESP8266 Boards > NodeMCU 1.0 (ESP-12E Module)*.
4. **Exceptions C++ (obligatoire).** Choisir *Outils > C++ Exceptions > **Enabled***. Sans cette option, la compilation s'arrête volontairement sur un `#error` explicite.
5. **Bibliothèque du projet.** Rendre `libraries/PeripheriquesIoT` visible par l'IDE. La commande crée un lien et non une copie, donc les modifications du dépôt sont prises en compte directement.
   - Windows (invite de commandes) :
     ```
     mklink /J "%USERPROFILE%\Documents\Arduino\libraries\PeripheriquesIoT" "<chemin du dépôt>\libraries\PeripheriquesIoT"
     ```
   - Linux :
     ```
     ln -s "<chemin du dépôt>/libraries/PeripheriquesIoT" ~/Arduino/libraries/
     ```

### Bibliothèques utilisées
| Bibliothèque | Version | Origine |
|---|---|---|
| ESP8266 core (`ESP8266WiFi`, `ESP8266WebServer`, `Servo`) | 3.1.2 | gestionnaire de cartes |
| PeripheriquesIoT | 0.1.0 | ce dépôt |
| WebSockets (Markus Sattler) | à figer en séance 2 | gestionnaire de bibliothèques |

## Compilation et téléversement
- **IDE** : ouvrir `projet_voiture_sergeev_osmane/projet_voiture_sergeev_osmane.ino`, puis *Téléverser*. La vitesse de téléversement peut aller jusqu'à 921600.
- **Programmes de test** : *Fichier > Exemples > PeripheriquesIoT > TestLed, TestBuzzer, TestServo, TestUltrason*.
- **En ligne de commande** : les scripts compilent l'application et tous les exemples, et échouent si nos fichiers produisent le moindre warning.
  - Linux ou Git Bash :
    ```bash
    outils/compiler.sh
    ```
  - PowerShell :
    ```
    powershell -ExecutionPolicy Bypass -File outils\compiler.ps1
    ```

  FQBN utilisé : `esp8266:esp8266:nodemcuv2:exception=enabled`.
- Moniteur série : **115200 bauds**. Dans l'application, la liaison est en émission seule, car RX sert aux feux de recul.

## Utilisation
*À compléter en séances 2 et 3* : réseau Wi-Fi `VoitureRC`, page `http://192.168.4.1`, commandes et réactions de la voiture. Le protocole est décrit dans [docs/protocole.md](docs/protocole.md).

## Archive des sources
```bash
git archive --format=zip --prefix=projet_voiture_sergeev_osmane/ -o projet_voiture_sergeev_osmane_sources.zip HEAD
```

## Documentation
[Architecture](docs/architecture.md) · [Cas d'utilisation](docs/cas_utilisation.md) · [Classes](docs/diagramme_classes.md) · [Séquence](docs/diagramme_sequence.md) · [Câblage](docs/cablage.md) · [Protocole](docs/protocole.md) · [Planning](docs/planning.md) · [Journal](docs/journal.md)
