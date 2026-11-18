# Planning — 4 séances de 2 h 45

| Séance | BE | Encadrant | Jalon attendu par le sujet | Tag |
|---|---|---|---|---|
| 1 | n°2 | non (autonomie) | bibliothèque bien avancée | `seance-1` |
| 2 | n°3 | **oui** | **architecture et cas d'utilisation validés**, bibliothèque terminée, début de l'application | `seance-2` |
| 3 | n°4 | non (autonomie) | application complète sur table | `seance-3` |
| 4 | n°5 | **oui** | **application et rapport terminés, version finale sur GitHub** | `seance-4`, `v1.0` |
| — | n°6 | oui | QCM, démonstration | — |

Règle commune : chaque séance se termine par une version qui **compile sans warning** et qui **marche sur le matériel**, même partiellement. On pose ensuite le tag, on pousse, et on complète `docs/journal.md`.
Dans le binôme : **A** et **B** désignent les deux membres. On répartit selon les affinités, mais en gardant les lots indépendants.

---

## Séance 1 (BE n°2, autonomie) — dépôt, outillage et composants du kit

**Objectifs**
- Dépôt structuré, chaîne de compilation vérifiée et brochage contrôlé au multimètre.
- Classes de base de la bibliothèque et exceptions.
- LED, buzzer, servo et ultrason fonctionnels, chacun avec son exemple de test.

**Tâches, dans l'ordre**
1. Renommer `project_name/` en `projet_voiture_sergeev_osmane/` avec `git mv`. Ajouter `.gitignore` et un squelette de README. Créer `libraries/PeripheriquesIoT/` (`library.properties`, `src/`, `examples/`) et le lien vers le dossier `libraries` de l'IDE.
2. Installer arduino-cli, le core esp8266 3.1.2 et l'option *C++ Exceptions*. Écrire `outils/compiler.sh` et `outils/compiler.ps1`, puis compiler le sketch vide.
3. Faire les vérifications au multimètre de [câblage §6](cablage.md#6-vérifications-à-faire-au-multimètre-séance-1-avant-de-brancher-le-moteur) : VCC des ports Grove, continuité, fil RX.
4. Écrire `Config.h` avec toutes les constantes du projet, même celles qui serviront plus tard.
5. Écrire `Erreurs.h` (avec un `#error` si les exceptions sont désactivées), `Peripherique` (vérification des broches et `operator<<`), `Actionneur` et `Capteur`.
6. Écrire `Led` et `TestLed` : phares, feux de recul et LED d'état, avec clignotement non bloquant.
7. Écrire `Buzzer` et `TestBuzzer` : bip, bip périodique, mode continu, le tout sans `delay()`.
8. Écrire `Servomoteur` et `TestServo` : neutre au démarrage, balayage de −100 à +100 %, calibration de l'amplitude.
9. Écrire `FiltreMedian`, `CapteurUltrason` (avec ISR) et `TestUltrason` : distance affichée 10 fois par seconde, détection du débranchement.
10. Compiler l'ensemble, faire les tests sur le matériel, puis commit, tag `seance-1`, push et mise à jour du journal.

**Lots en parallèle**
| Lot A | Lot B |
|---|---|
| Tâche 1 (structure du dépôt), puis 5 (classes de base, **à pousser en premier**, au bout d'environ 30 min), 6 et 7 | Tâches 2 et 3 (outillage, multimètre), puis 4 (`Config.h`), 8 et 9 |

Point de synchronisation : les en-têtes de `Peripherique`, `Capteur` et `Actionneur` sont recopiés du diagramme de classes avant le début des travaux. B peut donc coder contre ces en-têtes pendant que A écrit les `.cpp`.

**Fichiers** : `.gitignore`, `README.md`, `projet_voiture_sergeev_osmane/*` (renommage), `projet_voiture_sergeev_osmane/Config.h`, `libraries/PeripheriquesIoT/library.properties`, `src/{PeripheriquesIoT.h, Erreurs.h, Peripherique.*, Actionneur.h, Capteur.*, Led.*, Buzzer.*, Servomoteur.*, FiltreMedian.h, CapteurUltrason.*}`, `examples/Test{Led,Buzzer,Servo,Ultrason}/`, `outils/compiler.{sh,ps1}`, `docs/journal.md`.

**Critère de fin, vérifiable sur le matériel**
- `outils/compiler.sh` compile `projet_voiture_sergeev_osmane` et les 4 exemples, **sans aucun warning**.
- TestLed : la LED d'état clignote à 1 Hz, phares et feux s'allument et s'éteignent en alternance, et la carte démarre normalement avec les LED branchées sur D3 et RX.
- TestBuzzer : 3 bips, puis un bip dont la période passe de 500 à 100 ms.
- TestServo : le servo est au neutre dès la mise sous tension, sans à-coup, puis balaie l'amplitude.
- TestUltrason : on compare à une règle à 10, 30 et 100 cm (±2 cm attendus). Si on débranche le câble, le message « ErreurMesure » s'affiche **sans redémarrage de la carte**.
- Le port série affiche l'inventaire `Serial << led << buzzer << ...`.

---

## Séance 2 (BE n°3, avec l'encadrant) — validation, moteur et premier pilotage de bout en bout

**Objectifs**
- Faire valider l'architecture et les cas d'utilisation par l'encadrant, avec [la fiche d'une page](presentation_encadrant.md).
- **Terminer la bibliothèque** avec le moteur.
- Piloter direction, marche avant et marche arrière depuis le téléphone.

**Tâches, dans l'ordre**
1. Présenter l'architecture à l'encadrant (10 min), noter ses remarques dans le journal et ajuster les diagrammes si besoin.
2. Écrire `MoteurCC` (rampe, pause d'inversion, arrêt immédiat) et `MoteurBTS7960` (PWM sur RPWM et LPWM, EN).
3. Écrire `TestMoteur`. **Sans le BTS7960**, on observe 2 LED sur D1 et D2 : la luminosité suit la rampe et on voit la pause éteinte avant chaque inversion. **Avec le BTS7960**, roues en l'air, en suivant la séquence de mise sous tension du câblage.
4. Installer la bibliothèque WebSockets et figer sa version dans le README.
5. Écrire `Protocole` (`Commande`, `decoder()`, `ErreurProtocole`) et un test par le port série (on tape `C 50 -20` dans le moniteur série). Remarque : ce test lit la console série, ce qui demande RX ; on le fait avant de brancher les feux de recul sur RX.
6. Écrire `ServeurPilotage` : `softAP`, HTTP sur le port 80, WebSocket sur le port 81, rappels `std::function`.
7. Écrire un `PageWeb.h` minimal : deux curseurs (vitesse, direction) et l'envoi de `C v d` à 10 Hz.
8. Dans `Application`, relier les commandes au moteur et au servo, sans encore la classe `Voiture`. Ajouter un chien de garde minimal : arrêt si aucune commande depuis 500 ms.
9. Faire les tests de bout en bout, puis commit, tag `seance-2`, push et mise à jour du journal.

**Lots en parallèle**
| Lot A | Lot B |
|---|---|
| Tâches 2 et 3 (moteur et banc de test), puis câblage du BTS7960 s'il est disponible | Tâches 4 à 7 (protocole, serveur, page minimale) |

Tâche 1 ensemble, en début de séance. Tâches 8 et 9 ensemble, en fin de séance.

**Fichiers** : `src/{MoteurCC.*, MoteurBTS7960.*}`, `examples/TestMoteur/`, `projet_voiture_sergeev_osmane/{Protocole.*, ServeurPilotage.*, PageWeb.h, Application.*}`, `README.md` (bibliothèques), `docs/*` (remarques de l'encadrant), `docs/journal.md`.

**Critère de fin, vérifiable sur le matériel**
- Le téléphone voit le réseau `VoitureRC`, s'y connecte et `http://192.168.4.1` affiche la page.
- Le curseur de direction déplace le servo en direct.
- Le curseur de vitesse fait tourner le moteur, ou allume les LED témoins, dans les deux sens, avec une rampe visible et un passage par l'arrêt à l'inversion.
- Quand on ferme l'onglet ou qu'on coupe le Wi-Fi du téléphone, tout s'arrête en moins de 0,5 s.
- La trame `X 99` envoyée depuis la console du navigateur produit une réponse `err` et rien ne bouge.

---

## Séance 3 (BE n°4, autonomie) — application complète

**Objectifs**
- Écrire la classe `Voiture` avec la logique de recul et toutes les sécurités.
- Finaliser la page web, utilisable au pouce, avec l'affichage de l'état.
- Commencer le rapport.

**Tâches, dans l'ordre**
1. Écrire `Voiture` : composition des périphériques, `std::vector<Peripherique*>`, `begin()`, `update()`, `miseEnSecurite()`, `afficherInventaire()`.
2. Écrire `Voiture::appliquer()` : conduite, phares, klaxon et seuil, avec `Commande::operator!=` pour ne journaliser que les changements.
3. Écrire `Voiture::gererRecul()` : feux automatiques, table `std::map` distance → période du bip, limitation à 30 % sous 60 cm, blocage sous le seuil avec hystérésis, mode dégradé sur `ErreurMesure`.
4. Regrouper les sécurités : chien de garde, déconnexion, mode SÉCURITÉ, motif SOS sur la LED d'état si une `ErreurBroche` survient à l'initialisation.
5. Écrire `Protocole::encoder()` (JSON d'état) et diffuser l'état à 5 Hz.
6. Finaliser la page : pédale verticale et volant horizontal multi-touch avec retour au neutre, boutons phares et klaxon, curseur de seuil, jauge de distance colorée, bandeau de mode, indicateur de liaison, reconnexion automatique, gestion de `visibilitychange`.
7. Faire passer les scénarios S1 à S3 sur table, avec une main ou un carton comme obstacle.
8. Rédiger le squelette de `docs/rapport.md` (introduction, cas d'utilisation, conception), puis commit, tag `seance-3`, push et mise à jour du journal.

**Lots en parallèle**
| Lot A | Lot B |
|---|---|
| Tâches 1 à 4 (`Voiture` et sécurités). Les tests passent par la console série, en injectant des `Commande`. | Tâches 5 et 6 (état JSON et page finale), puis tâche 8 (rapport) |

**Fichiers** : `projet_voiture_sergeev_osmane/{Voiture.*, Application.*, Protocole.*, PageWeb.h, Config.h}`, `docs/{rapport.md, journal.md, diagramme_*.md}`.

**Critère de fin, vérifiable sur le matériel**
- **S2** : phares au bouton, klaxon audible, vitesse et direction au pouce, téléphone verrouillé entraînant un arrêt en moins de 0,5 s.
- **S3** : en reculant vers une main, les feux de recul s'allument, le bip accélère de façon audible, l'arrêt se fait au seuil (vérifié à la règle, ±3 cm) et la marche avant reste possible.
- Après avoir réglé le seuil à 40 cm dans la page, la voiture s'arrête à environ 40 cm.
- Quand on débranche l'ultrason, la page affiche « capteur KO », le recul est refusé et la marche avant reste permise.

---

## Séance 4 (BE n°5, avec l'encadrant) — intégration, robustesse et livrables

**Objectifs**
- Faire rouler la voiture complète au sol.
- Livrer le code propre, le README, le rapport et les diagrammes à jour.
- Pousser la version finale et prévenir l'encadrant.

**Tâches, dans l'ordre**
1. Intégrer sur le châssis : batterie, interrupteur, fusible, convertisseur 5 V, masse commune, fixation du capteur à l'arrière.
2. Faire les essais au sol. Régler la pente de la rampe, l'amplitude du servo, la vitesse maximale de recul et la table du bip.
3. Travailler la robustesse : tenue aux redémarrages par chute de tension (condensateur sur le servo), reconnexion du téléphone, essai de 10 minutes sans plantage, vérification que le tas mémoire ne diminue pas (`ESP.getFreeHeap()` dans les traces).
4. Nettoyer le code : commentaires Doxygen complets, aucune valeur magique, `grep delay` vide hors exemples, aucun warning.
5. Finaliser le README : matériel, câblage, installation, compilation, Wi-Fi, mode d'emploi.
6. Mettre à jour le rapport (3 pages maximum une fois exporté) et les diagrammes, au plus près du code réel.
7. Générer l'archive avec `git archive`, puis tags `seance-4` et `v1.0`, push, et message à l'encadrant.

**Lots en parallèle**
| Lot A | Lot B |
|---|---|
| Tâches 1 à 3 (intégration, essais, réglages, robustesse) | Tâches 4 à 6 (nettoyage, README, rapport et diagrammes), puis 7 ensemble |

**Fichiers** : `projet_voiture_sergeev_osmane/Config.h` (réglages), corrections ponctuelles, `README.md`, `docs/{rapport.md, journal.md, diagramme_*.md, cablage.md}`.

**Critère de fin, vérifiable sur le matériel**
- Les 3 scénarios passent **au sol**, devant l'encadrant.
- La voiture roule 10 minutes sans redémarrage.
- Le rapport fait 3 pages au plus une fois exporté en PDF.
- `git archive` produit le zip et les tags sont visibles sur GitHub.

---

## Gestion des risques
| Risque | Parade |
|---|---|
| BTS7960 ou moteur livrés tard | Moteur simulé par 2 LED, ce qui permet de tester toute la logique (rampe, inversion, recul). Le matériel réel ne demande ensuite que la séquence de mise sous tension. |
| Ports Grove en 5 V | Pont diviseur sur SIG de l'ultrason (§6 du câblage) |
| Problème de démarrage lié à D3 ou RX | Plan de repli du câblage (EN relié au 3V3, feux sur D8), qui ne demande qu'un changement dans `Config.h` |
| Retard | Ordre de priorité : pilotage de base, puis sécurités, puis recul assisté, puis finitions de la page. Les dernières finitions de la page sont les premières sacrifiées. |
