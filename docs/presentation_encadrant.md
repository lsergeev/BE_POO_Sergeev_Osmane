# Voiture RC pilotée par téléphone — fiche de validation (séance BE n°3)

**Idée.** Une voiture radiocommandée, avec un moteur de 30 W et un servo de direction, pilotée depuis le **navigateur d'un téléphone**, sans application à installer. Elle intègre une **aide au recul** comme une vraie voiture : feux de recul, bip qui accélère à l'approche d'un obstacle et freinage automatique sous un seuil réglable.

**Cas d'utilisation.**
- *Conducteur* : se connecter, avancer ou reculer (vitesse proportionnelle), diriger, phares, klaxon, suivre la distance et l'état, régler le seuil.
- *Automatique* : assister le recul, mettre en sécurité (liaison perdue en 500 ms, capteur KO).

Scénarios : S1 démarrage et connexion · S2 conduite avec phares · S3 marche arrière avec obstacle.

**Architecture en 2 couches.**

| Bibliothèque `PeripheriquesIoT` (générique, ses exemples servent de tests) | Application `projet_voiture_sergeev_osmane` (sketch du template) |
|---|---|
| `Peripherique` ◁ `Capteur` ◁ `CapteurUltrason` (ISR et `FiltreMedian`) | `Application` : `init()` / `run()` du template, chef d'orchestre |
| `Peripherique` ◁ `Actionneur` ◁ `Led`, `Buzzer`, `Servomoteur`, `MoteurCC` ◁ `MoteurBTS7960` | `Voiture` : composition des périphériques, recul assisté, sécurités |
| `ErreurPeripherique` ◁ `ErreurBroche`, `ErreurMesure` (dérivent de `std::runtime_error`) | `ServeurPilotage` (AP, HTTP, WebSocket), `Protocole`, `ErreurProtocole`, `Config.h` |

**Exigences C++, chacune avec un usage concret.**

| Exigence | Usage dans le projet |
|---|---|
| Classes et héritage | Environ 18 classes. Hiérarchie de périphériques sur 3 niveaux. La rampe est codée dans `MoteurCC` et le PWM dans `MoteurBTS7960` (patron *méthode modèle*). |
| Opérateur | `Print& operator<<(Print&, const Peripherique&)`, qui appelle la méthode virtuelle `decrire()` : `Serial << moteur << radar` sert à l'inventaire et aux tests. `Commande::operator!=` sert à ne journaliser que les changements. |
| STL | `vector<Peripherique*>` pour l'initialisation, la mise à jour et la mise en sécurité polymorphes. `map<distance, période>` avec `lower_bound` pour le bip. `array` et `nth_element` pour le filtre médian. `function` pour les rappels du serveur. |
| Exceptions | `ErreurBroche` : broche invalide, le moteur reste désactivé. `ErreurMesure` : capteur KO, le recul est interdit. `ErreurProtocole` : trame invalide, elle est ignorée. |

**Choix techniques.**
- **Mode AP** : indépendant du réseau de l'INSA, avec une adresse fixe `192.168.4.1`.
- **WebSocket** plutôt que des requêtes HTTP répétées :
  - une trame de quelques octets au lieu de centaines d'octets d'en-têtes ;
  - la distance est poussée par l'ESP ;
  - la déconnexion est détectée et entraîne un arrêt immédiat.
- **Sécurités** :
  - chien de garde de 500 ms ;
  - rampe ;
  - pause à zéro avant chaque inversion ;
  - neutre au démarrage ;
  - pont en H désactivé pendant le boot grâce à GPIO15.
- **Code non bloquant** : aucun `delay()` et ultrason mesuré par interruption.
- **Une seule bibliothèque externe** : WebSockets.

**Brochage.**

| Broche | Fonction |
|---|---|
| D1 / D2 | RPWM / LPWM |
| D8 | EN, à 0 au boot |
| D5 | servo |
| D6 | ultrason |
| D7 | buzzer |
| D3 | phares, actifs à l'état bas pour ne pas gêner le boot |
| RX | feux de recul, port série en émission seule |
| D4 | LED d'état |

Alimentation : batterie moteur séparée, 5 V pour le servo, logique en 3,3 V partout et masse commune.

**Planning.**
- S1 : composants du kit et classes de base.
- S2 : moteur et premier pilotage de bout en bout.
- S3 : application complète.
- S4 : intégration, rapport et version finale.

Chaque séance se termine par une version qui marche, avec un tag git.

**Questions à l'encadrant.**
1. La hiérarchie (3 niveaux) et le placement de la rampe dans la bibliothèque vous conviennent-ils ?
2. L'usage de `operator<<` est-il suffisant pour l'exigence « redéfinition d'opérateur » ?
3. Le mode AP est-il acceptable pour la démonstration ?
4. Y a-t-il une consigne particulière pour le moteur de 30 W en salle (banc, roues en l'air) ?
