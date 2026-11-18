# Architecture — Voiture RC pilotée par téléphone (ESP8266)

> Document de l'étape 0, à valider avec l'encadrant lors de la séance 2 (BE n°3).
> Documents liés : [cas d'utilisation](cas_utilisation.md) · [diagramme de classes](diagramme_classes.md) ·
> [diagramme de séquence](diagramme_sequence.md) · [câblage](cablage.md) · [protocole](protocole.md) ·
> [planning](planning.md) · [fiche encadrant](presentation_encadrant.md)

## 1. Vue d'ensemble

```mermaid
flowchart LR
    tel["📱 Téléphone<br/>page web (HTML/JS)"] -- "Wi-Fi « VoitureRC »<br/>(point d'accès de l'ESP)" --- esp
    subgraph esp["ESP8266 NodeMCU + shield Grove"]
        direction TB
        app["<b>Application</b> (sketch projet_voiture_sergeev_osmane)<br/>Application · Voiture · ServeurPilotage · Protocole"]
        lib["<b>Bibliothèque PeripheriquesIoT</b><br/>Led · Buzzer · Servomoteur · CapteurUltrason · MoteurBTS7960"]
        app --> lib
    end
    lib --> hw["Moteur 30 W (BTS7960) · Servo direction<br/>Ultrason arrière · Buzzer · 4 LED"]
```

Deux couches strictement séparées :

| Couche | Emplacement | Rôle | Dépend de |
|---|---|---|---|
| **Bibliothèque** `PeripheriquesIoT` | `libraries/PeripheriquesIoT/` (vraie bibliothèque Arduino, avec `library.properties` et `examples/`) | Capteurs et actionneurs **génériques** et réutilisables. Ne connaît ni la voiture, ni le Wi-Fi, ni `Config.h` : tous les paramètres (broches, pentes, seuils) arrivent par les constructeurs. | Core ESP8266 (`Arduino.h`, `Servo`) |
| **Application** `projet_voiture_sergeev_osmane` | `projet_voiture_sergeev_osmane/` (ex-`project_name/` du template) | Logique de la voiture : conduite, recul assisté, sécurités, serveur web et WebSocket, page. | Bibliothèque + `WebSockets` + `ESP8266WiFi`/`ESP8266WebServer` |

La règle est simple et se vérifie avec un `grep` : aucun fichier de `libraries/` n'inclut un fichier de `projet_voiture_sergeev_osmane/`.
Les programmes de test de chaque composant sont les **exemples de la bibliothèque** (`examples/TestLed`, …).
Ils sont accessibles dans l'IDE par *Fichier > Exemples > PeripheriquesIoT*, ce qui montre que la bibliothèque s'utilise seule.

## 2. Tableau de traçabilité

### 2.1 Barème

| Ligne du barème | Pts | Réponse du projet | Où le voir | Séance |
|---|---|---|---|---|
| Projet qui fonctionne | 4 | Une version qui compile et qui marche à la fin de **chaque** séance. Le moteur est simulé par 2 LED tant que le BTS7960 n'est pas câblé. La démonstration suit les 3 scénarios. | tags `seance-1` … `seance-4`, [planning](planning.md) | 1→4 |
| Principes objet | 2 | Encapsulation (attributs privés ou protégés, accesseurs `const`), abstraction (`Peripherique`, `Capteur`, `Actionneur`, `MoteurCC`), polymorphisme (`begin()`, `update()`, `arreter()`, `decrire()` virtuels), composition (`Voiture` possède ses périphériques), patron « méthode modèle » (rampe dans `MoteurCC`, PWM dans `MoteurBTS7960`). | [diagramme de classes](diagramme_classes.md) | 1–3 |
| Plusieurs classes | 1 | Environ 12 classes dans la bibliothèque et 6 dans l'application, chacune avec ses fichiers `.h` et `.cpp`. | `libraries/…/src`, `projet_voiture_sergeev_osmane/` | 1–3 |
| Héritage | 1 | `Peripherique` → `Capteur` / `Actionneur` → `Led`, `Buzzer`, `Servomoteur`, `CapteurUltrason`, `MoteurCC` → `MoteurBTS7960`. Côté exceptions : `std::runtime_error` → `ErreurPeripherique` → `ErreurBroche` et `ErreurMesure`, plus `ErreurProtocole`. | `Peripherique.h`, `Erreurs.h` | 1–2 |
| Redéfinition d'opérateur | 1 | `Print& operator<<(Print&, const Peripherique&)` affiche n'importe quel périphérique sur le port série (`Serial << moteur << radar;`). Il s'appuie sur la méthode virtuelle `decrire()` et sert à l'inventaire au démarrage et aux tests. En complément, `Commande::operator==` et `operator!=` permettent de ne journaliser que les changements de commande. | `Peripherique.h`, `Protocole.h` | 1, 3 |
| STL | 1 | `std::vector<Peripherique*>` pour initialiser, mettre à jour et mettre en sécurité tous les périphériques d'une seule boucle. `std::map<uint16_t,uint16_t>` relie une distance à une période de bip, avec une recherche par `lower_bound`. `std::array` et `std::nth_element` forment le filtre médian des mesures ultrason. `std::function` sert aux rappels du serveur vers l'application. `std::clamp` borne les consignes. | `Voiture.cpp`, `FiltreMedian.h`, `ServeurPilotage.h` | 1–3 |
| Exceptions | 1 | `ErreurBroche` : broche interdite ou sans PWM, levée à la construction et attrapée dans `Application::init()`. Le moteur reste alors désactivé et la LED d'état clignote en SOS. `ErreurMesure` : capteur ultrason absent ou muet, levée par `distanceCm()` et attrapée par `Voiture`. Le recul est alors **interdit**, ce qui garantit une sécurité par défaut. `ErreurProtocole` : message WebSocket invalide, levée par `Protocole::decoder()` et attrapée par `ServeurPilotage`. Le message est ignoré, une erreur est renvoyée et le programme ne plante pas. | `Erreurs.h`, `Application.cpp`, `Voiture.cpp` | 1–3 |
| Programme propre et commenté | 2 | En-têtes Doxygen en français (format du template), une classe par responsabilité, aucune valeur magique (tout est dans `Config.h`), compilation sans warning (`arduino-cli --warnings all`), aucun `delay()` dans `loop()`. | tout le code | 1–4 |
| Qualité et quantité | 2 | Bibliothèque réutilisable avec ses exemples, plus sécurités, page web soignée, filtrage des mesures, mode dégradé et hystérésis. | — | 1–4 |
| Idée intéressante | 2 | Une voiture RC avec aide au recul « comme une vraie voiture » : feux de recul, bip qui accélère, freinage automatique. Le pilotage se fait sans application à installer, depuis le navigateur du téléphone. | [cas d'utilisation](cas_utilisation.md) | 3 |
| Rapport | 3 | `docs/rapport.md`, 3 pages maximum, alimenté au fil de l'eau par `docs/journal.md`. | `docs/` | 3–4 |
| Échéances | −5 | Un tag git et un push à la fin de chaque séance, version finale poussée en séance 4 (BE n°5). | `git tag` | 1–4 |

### 2.2 Exigences techniques de l'énoncé et du cahier des charges

| Exigence | Réponse | Où |
|---|---|---|
| ESP8266 NodeMCU, IDE Arduino, shield Grove | Carte NodeMCU 1.0 (ESP-12E), core esp8266 3.1.2, modules du kit sur les ports Grove | [câblage](cablage.md) |
| Bibliothèque modulaire et extensible, séparée de l'application | Bibliothèque Arduino autonome `libraries/PeripheriquesIoT`. Ajouter un capteur revient à dériver `Capteur` et implémenter `acquerir()`. Ajouter un autre pont en H revient à dériver `MoteurCC` et implémenter `appliquer()`. | §1 |
| Point d'accès Wi-Fi (mode AP) | `WiFi.softAP()`, page à l'adresse fixe `http://192.168.4.1`. Justification au §3.1. | `ServeurPilotage` |
| WebSocket plutôt que des requêtes HTTP répétées | Justification au §3.2. | [protocole](protocole.md) |
| Avant et arrière proportionnels, direction | `C v d` envoyé à 10 Hz, v et d entre −100 et 100 % | `Voiture::appliquer()` |
| Phares, klaxon | `P 0` ou `P 1`, `K` | idem |
| Distance arrière et état en direct | État JSON diffusé à 5 Hz | `Protocole::encoder()` |
| Feux de recul automatiques | Allumés tant que la vitesse ou la consigne est négative | `Voiture::gererRecul()` |
| Bip qui accélère quand l'obstacle approche | Table `std::map` qui relie une distance à une période | idem |
| Arrêt du recul sous un seuil réglable | `S n`, entre 10 et 100 cm, avec une hystérésis de 5 cm | idem |
| Arrêt si aucune commande pendant X ms | `TIMEOUT_COMMANDE_MS = 500` (chien de garde logiciel), plus un arrêt immédiat à la déconnexion WebSocket | `Voiture::update()` |
| Rampe d'accélération | `MoteurCC` limite la pente (%/s), réglable dans `Config.h` | `MoteurCC::update()` |
| Passage par l'arrêt avant chaque changement de sens | `MoteurCC` impose une vitesse nulle pendant `PAUSE_INVERSION_MS` avant d'inverser | idem |
| Direction au neutre au démarrage | `Servomoteur::begin()` utilise `attach(pin, min, max, neutre)`, si bien que la première impulsion est déjà au neutre | `Servomoteur.cpp` |
| Code non bloquant | Pas de `delay()`. Tout est cadencé par `millis()`. La mesure ultrason se fait **par interruption** (pas de `pulseIn` bloquant). | partout |
| 3,3 V, broches de boot, GPIO16 | Table de brochage justifiée, vérification au multimètre en séance 1 | [câblage](cablage.md) |
| Alimentation : batterie séparée, masse commune, 5 V pour le servo | Schéma d'alimentation | [câblage](cablage.md) |
| Exceptions activées | *Outils > C++ Exceptions > Enabled*, plus un `#error` explicite si on oublie (test de `__cpp_exceptions`) | `Erreurs.h`, README |
| Peu de bibliothèques | Une seule bibliothèque externe : **WebSockets** (Markus Sattler). Le reste vient du core (`ESP8266WiFi`, `ESP8266WebServer`, `Servo`). Pas d'ArduinoJson : le JSON d'état est produit avec `snprintf`. | README |

## 3. Choix de conception à justifier

### 3.1 Point d'accès Wi-Fi sur l'ESP (mode AP)
- Les cartes ne sont pas déclarées sur le réseau IoT de l'INSA. Le mode AP ne dépend d'**aucune** infrastructure.
- L'adresse est **fixe et connue** (`192.168.4.1`). Avec un partage de connexion du téléphone, il faudrait retrouver l'IP de l'ESP, qui change d'une fois à l'autre.
- La démo est autonome partout : salle de TP, couloir, extérieur.
- La contrepartie : le téléphone perd l'accès à Internet pendant le pilotage, ce qui est sans importance ici. Le nombre de clients est limité à 1 conducteur (`max_connection`), ce qui évite deux pilotes en même temps.

### 3.2 WebSocket plutôt que des requêtes HTTP répétées
- **Latence et surcoût** : une requête HTTP transporte plusieurs centaines d'octets d'en-têtes et ouvre souvent une connexion TCP. Une trame WebSocket ajoute 2 à 6 octets à un message de quelques caractères.
- **Bidirectionnel** : l'ESP *pousse* la distance et l'état à 5 Hz, sans que la page ait à interroger en boucle.
- **Détection de perte de liaison** : l'événement de déconnexion déclenche un arrêt immédiat. Il complète le chien de garde de 500 ms.
- **Charge de l'ESP** : `ESP8266WebServer` traite les requêtes une par une. Dix requêtes HTTP par seconde provoqueraient de la gigue et retarderaient la boucle.
- Le HTTP sert uniquement à livrer la page, une fois, au chargement.

### 3.3 Où placer la rampe et la pause d'inversion ?
Elles sont dans la **bibliothèque** (`MoteurCC`), pas dans l'application. Elles protègent le *matériel* (moteur et pont en H) quelle que soit l'application qui l'utilise. `MoteurCC` calcule la vitesse courante (rampe, pause à zéro). Seule la méthode virtuelle pure `appliquer(vitesse)` dépend du pont : c'est le patron *méthode modèle*.

### 3.4 Mesure ultrason non bloquante
`pulseIn()` peut bloquer jusqu'à 30 ms sans obstacle, ce qui est incompatible avec un serveur réactif.
`CapteurUltrason` envoie l'impulsion de déclenchement (10 µs, négligeable), repasse la broche en entrée et mesure l'écho **par interruption** (`attachInterruptArg`, fronts montant et descendant, horodatage avec `micros()`). `update()` récupère le résultat ou constate un dépassement de délai.
Absence de front montant = capteur absent → `ErreurMesure` après N échecs.
Impulsion longue = hors de portée → distance maximale, mesure **valide**.
Une médiane sur 5 valeurs (`FiltreMedian`) élimine les échos parasites.

### 3.5 Sécurité par défaut
- Au démarrage, `R_EN` et `L_EN` sont sur GPIO15, qui est tirée à la masse au boot : le pont est **désactivé** tant que le programme ne l'a pas validé. La direction est au neutre et la consigne à 0.
- En cas de perte de liaison (déconnexion ou plus de commande depuis 500 ms), la consigne passe à 0 et le klaxon et le bip sont coupés. La voiture redevient pilotable dès qu'une nouvelle commande arrive.
- Si le capteur arrière est en défaut, le recul est **interdit** (marche avant seulement) et la page affiche « capteur KO ».
- Sous le seuil en recul, la voiture s'arrête **immédiatement**, sans rampe pour ne pas dépasser le seuil. Elle reste bloquée en recul tant que la distance n'a pas dépassé seuil + hystérésis. La marche avant reste toujours possible.

## 4. Arborescence du dépôt

Le dossier `project_name/` du template est renommé `projet_voiture_sergeev_osmane/`, car l'IDE Arduino exige que le dossier porte le même nom que le `.ino`. On conserve le squelette du template : `projet_voiture_sergeev_osmane.ino` appelle `Application::init()` et `Application::run()`, avec le même format d'en-tête Doxygen.

```
BE_POO_Template/
├── README.md                      compilation, téléversement, utilisation
├── .gitignore
├── projet_voiture_sergeev_osmane/   ── APPLICATION (sketch Arduino, ex-project_name/)
│   ├── projet_voiture_sergeev_osmane.ino   setup() → app.init() ; loop() → app.run()  (inchangé du template)
│   ├── Application.h / .cpp       chef d'orchestre : relie Voiture et ServeurPilotage
│   ├── Config.h                   TOUTES les constantes : broches, seuils, délais, Wi-Fi
│   ├── Voiture.h / .cpp           logique de conduite, recul assisté, sécurités
│   ├── ServeurPilotage.h / .cpp   point d'accès, HTTP (page), WebSocket
│   ├── Protocole.h / .cpp         Commande, EtatVoiture, décodage/encodage, ErreurProtocole
│   └── PageWeb.h                  page HTML/CSS/JS en PROGMEM (chaîne brute R"(...)")
├── libraries/
│   └── PeripheriquesIoT/          ── BIBLIOTHÈQUE (indépendante de l'application)
│       ├── library.properties
│       ├── src/
│       │   ├── PeripheriquesIoT.h      en-tête « parapluie »
│       │   ├── Erreurs.h               ErreurPeripherique, ErreurBroche, ErreurMesure
│       │   ├── Peripherique.h / .cpp   classe de base abstraite + operator<<
│       │   ├── Capteur.h / .cpp        base abstraite des capteurs (cadencement)
│       │   ├── Actionneur.h            base abstraite des actionneurs (arreter())
│       │   ├── Led.h / .cpp
│       │   ├── Buzzer.h / .cpp
│       │   ├── Servomoteur.h / .cpp
│       │   ├── CapteurUltrason.h / .cpp
│       │   ├── FiltreMedian.h          modèle (template) header-only
│       │   ├── MoteurCC.h / .cpp       rampe et pause d'inversion (abstraite)
│       │   └── MoteurBTS7960.h / .cpp
│       └── examples/              programmes de test, un par composant
│           ├── TestLed/TestLed.ino
│           ├── TestBuzzer/TestBuzzer.ino
│           ├── TestServo/TestServo.ino
│           ├── TestUltrason/TestUltrason.ino
│           └── TestMoteur/TestMoteur.ino
├── docs/
│   ├── architecture.md            (ce fichier) traçabilité, choix, arborescence
│   ├── cas_utilisation.md         cas d'utilisation et scénarios
│   ├── diagramme_classes.md       UML (Mermaid, rendu directement par GitHub)
│   ├── diagramme_sequence.md
│   ├── cablage.md                 brochage, alimentation, vérifications 3,3 V
│   ├── protocole.md               messages page ↔ ESP
│   ├── planning.md                4 séances
│   ├── presentation_encadrant.md  fiche d'une page pour la validation
│   ├── journal.md                 déroulement du projet, séance par séance
│   └── rapport.md                 rapport (3 pages maximum)
└── outils/
    ├── compiler.sh                arduino-cli : compile l'application et tous les exemples
    └── compiler.ps1               idem sous Windows
```

**Rendre la bibliothèque visible par l'IDE.** C'est une seule commande, qui crée un lien et non une copie : les modifications du dépôt sont donc prises en compte directement.
- Windows : `mklink /J "%USERPROFILE%\Documents\Arduino\libraries\PeripheriquesIoT" "<dépôt>\libraries\PeripheriquesIoT"`
- Linux (machines de l'INSA) : `ln -s "<dépôt>/libraries/PeripheriquesIoT" ~/Arduino/libraries/`
- arduino-cli : `--library libraries/PeripheriquesIoT`, sans aucune installation.

**Archive des sources** : `git archive --format=zip --prefix=projet_voiture_sergeev_osmane/ -o projet_voiture_sergeev_osmane_sources.zip HEAD`

## 5. Bibliothèques et outils

| Élément | Version | Rôle |
|---|---|---|
| Core ESP8266 pour Arduino | 3.1.2 | carte NodeMCU 1.0, `ESP8266WiFi`, `ESP8266WebServer`, `Servo` (inclus) |
| WebSockets (Markus Sattler / Links2004) | 2.7.x, version exacte figée à l'installation en séance 2 | serveur WebSocket sur le port 81 |
| arduino-cli | dernière version stable | vérification de la compilation à chaque séance |

FQBN de compilation : `esp8266:esp8266:nodemcuv2:exception=enabled` (nom exact de l'option à confirmer avec `arduino-cli board details` en séance 1).
