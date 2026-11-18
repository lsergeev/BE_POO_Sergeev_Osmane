# Journal de projet

Ce journal alimente la partie « déroulement du projet » du rapport. Une entrée par séance, avec ce qui a été fait, les problèmes rencontrés et les solutions trouvées.

## Étape 0 — Architecture (avant la séance 1)

**Fait**
- Lecture du sujet et du template : un sketch `project_name/` avec une classe `Application` qui expose `init()` et `run()`.
- Choix d'une architecture en deux couches : la bibliothèque Arduino autonome `libraries/PeripheriquesIoT`, dont les exemples servent de tests, et l'application `projet_voiture_sergeev_osmane/`.
- Rédaction de la traçabilité, des cas d'utilisation et scénarios, des diagrammes de classes et de séquence (Mermaid), du brochage, du protocole WebSocket et du planning.

**Points ouverts, à vérifier en séance 1**
- Tension VCC des ports Grove du shield, que la documentation ne précise pas.
- Fil RX du port UART du shield.
- Absence de rappel vers le haut sur `R_EN` du module BTS7960.
- Nom exact de l'option d'exceptions dans le FQBN arduino-cli.

## Séance 1 (BE n°2, autonomie) — dépôt, outillage, composants du kit

**Fait**
- Sketch du template renommé en `projet_voiture_sergeev_osmane/`, à la demande du binôme. Le dossier et le `.ino` doivent porter le même nom pour l'IDE Arduino.
- Ajout de `.gitignore`, d'un README de base et des scripts `outils/compiler.sh` et `outils/compiler.ps1`. Ces scripts compilent l'application et les 4 exemples avec arduino-cli, en `--warnings all`, et échouent au moindre warning dans nos fichiers.
- Installation du core esp8266 3.1.2. Le FQBN est confirmé : `esp8266:esp8266:nodemcuv2:exception=enabled`.
- Écriture de `Config.h` : toutes les constantes du projet, avec des `static_assert` sur les seuils et sur la longueur du mot de passe Wi-Fi.
- Bibliothèque `libraries/PeripheriquesIoT` :
  - `Erreurs.h` (`ErreurPeripherique`, `ErreurBroche`, `ErreurMesure`) ;
  - `Peripherique` avec `operator<<` ;
  - `Capteur` et `Actionneur` ;
  - `Led`, `Buzzer`, `Servomoteur`, `FiltreMedian` et `CapteurUltrason`.
- Exemples de test `TestLed`, `TestBuzzer`, `TestServo` et `TestUltrason`.
- Application : LED d'état qui clignote et inventaire série, avec un clignotement rapide si l'initialisation échoue.
- Résultat de compilation : 5 croquis sans warning. Environ 36 % de RAM et 27 % de flash utilisés.

**Problèmes rencontrés et solutions**
- *Exceptions désactivées par défaut dans le core.* Un oubli de l'option produisait une erreur peu claire. On a ajouté un `#error` explicite dans `Erreurs.h`, qui teste `__cpp_exceptions`. Vérifié : sans l'option, la compilation s'arrête avec le bon message.
- *Exception levée avant `setup()`.* Les objets globaux sont construits avant `setup()`, et une exception levée à ce moment ne peut pas être attrapée. Les constructeurs ne font donc aucune vérification. Tout est contrôlé dans `begin()`, sur le patron *interface non virtuelle* : `verifierConfiguration()`, puis `initialiser()`. Le diagramme de classes a été mis à jour.
- *Ultrason sans `pulseIn`.* `pulseIn` bloque jusqu'à 30 ms. La mesure se fait maintenant par interruption sur les deux fronts (`attachInterruptArg`, ISR en IRAM). L'interruption est détachée pendant l'impulsion de déclenchement, sinon notre propre signal serait pris pour un écho.
- *Absence d'obstacle ou capteur absent ?* Un écho plus long que la portée maximale signifie « hors de portée », ce qui est une mesure valide. L'absence de front montant signifie un échec. Après 5 échecs, `distanceCm()` lève `ErreurMesure`, pour que l'application ne croie jamais qu'il n'y a pas d'obstacle quand le capteur est mort.
- *Buzzer appelé à chaque tour de boucle.* `bipPeriodique()` et `clignoter()` ne font rien si la période ne change pas, et `tone()`/`noTone()` ne sont appelées qu'aux changements.

**Reste à vérifier sur le matériel** (procédure dans le compte rendu de séance)
- Tension VCC des ports Grove, fil RX du port UART, rappel sur `R_EN` : à mesurer au multimètre.
- Exemples sur la carte : distance à la règle, calibration du servo à reporter dans `Config.h`.
