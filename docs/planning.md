# Planning (4 séances de 2h45)

| Séance | BE | Encadrant | Attendu | Tag |
|---|---|---|---|---|
| 1 | n°2 | non | bibliothèque bien avancée | `seance-1` |
| 2 | n°3 | oui | architecture validée, bibliothèque finie, début de l'appli | `seance-2` |
| 3 | n°4 | non | appli complète sur table | `seance-3` |
| 4 | n°5 | oui | appli et rapport finis, version finale sur GitHub | `seance-4`, `v1.0` |

À chaque fin de séance : ça compile sans warning, ça marche sur la carte, tag, push, journal.
A et B, ce sont nous deux.

## Séance 1 : outils et composants du kit

1. Renommer le sketch, ajouter `.gitignore`, README, dossier de la bibliothèque.
2. Installer le core esp8266 3.1.2 et l'option exceptions, écrire les scripts de compilation.
3. Mesures au multimètre ([cablage.md](cablage.md) §5).
4. `Config.h`.
5. `Erreurs.h`, `Peripherique` (avec `operator<<`), `Capteur`, `Actionneur`.
6. `Led` + `TestLed`.
7. `Buzzer` + `TestBuzzer`.
8. `Servomoteur` + `TestServo`.
9. `FiltreMedian`, `CapteurUltrason` + `TestUltrason`.
10. Tout compiler, tester, tag.

En parallèle : A fait 1, 5 (à pousser vite), 6 et 7. B fait 2, 3, 4, 8 et 9.

Fini quand :
- les 5 programmes compilent sans warning ;
- les LEDs clignotent, et la carte redémarre bien avec les LEDs branchées sur D3 et RX ;
- le buzzer fait 3 bips puis un bip qui accélère ;
- le servo est droit à l'allumage, puis balaie ;
- l'ultrason est juste à ±2 cm (règle à 10, 30 et 100 cm), et si on débranche, « ErreurMesure » s'affiche sans reboot.

## Séance 2 : validation, moteur, premier pilotage

1. Faire valider l'archi par l'encadrant ([fiche](presentation_encadrant.md)).
2. `MoteurCC` (rampe, pause avant inversion) et `MoteurBTS7960`.
3. `TestMoteur`, d'abord avec 2 LEDs à la place du moteur, puis roues en l'air.
4. Installer WebSockets.
5. `Protocole` (décodage + `ErreurProtocole`), testé au moniteur série (avant de mettre les feux sur RX).
6. `ServeurPilotage` : point d'accès, HTTP, WebSocket.
7. Une première page : deux curseurs.
8. Relier les commandes au moteur et au servo dans `Application`, avec un chien de garde de 500 ms.
9. Test complet, tag.

En parallèle : A fait 2 et 3 (moteur). B fait 4 à 7 (réseau et page). 1, 8 et 9 à deux.

Fini quand :
- le téléphone se connecte à `VoitureRC` et la page s'affiche ;
- le servo suit le curseur ;
- le moteur (ou les LEDs) suit le curseur dans les deux sens, avec rampe et arrêt au changement de sens ;
- si on ferme l'onglet, tout s'arrête en moins de 0,5 s ;
- `X 99` envoyé depuis la console renvoie une erreur et rien ne bouge.

## Séance 3 : l'appli complète

1. `Voiture` : contient les périphériques, `update()`, mise en sécurité.
2. Conduite, phares, klaxon, seuil.
3. Recul : feux auto, bip (`std::map`), limite à 30 % sous 60 cm, blocage sous le seuil, recul interdit si capteur KO.
4. Sécurités : chien de garde, déconnexion, LED qui clignote vite si erreur.
5. État en JSON, 5 fois par seconde.
6. Page finale : pédale et volant au pouce, boutons, jauge de distance, reconnexion auto.
7. Dérouler S1 à S3 sur table.
8. Début du rapport, tag.

En parallèle : A fait 1 à 4 (`Voiture`). B fait 5, 6 et 8 (page et rapport).

Fini quand :
- phares, klaxon, vitesse et direction marchent depuis le téléphone ;
- en reculant vers une main : feux, bip qui accélère, arrêt au seuil (±3 cm) ;
- avec le seuil réglé à 40 cm, ça s'arrête vers 40 cm ;
- ultrason débranché : « capteur KO » sur la page, plus de recul.

## Séance 4 : intégration et rendu

1. Tout monter sur le châssis (batterie, fusible, 5 V, masse commune).
2. Essais au sol, réglages dans `Config.h`.
3. Robustesse : 10 min sans reboot, reconnexion, mémoire stable.
4. Nettoyage du code et des commentaires.
5. README final.
6. Rapport (3 pages max) et diagrammes à jour.
7. Archive, tags `seance-4` et `v1.0`, push, mail à l'encadrant.

En parallèle : A fait 1 à 3 (voiture). B fait 4 à 6 (rendu). 7 à deux.

Fini quand : les 3 scénarios passent au sol, 10 min sans reboot, rapport ≤ 3 pages, tags sur GitHub.

## Si ça se passe mal :

- Le moteur arrive tard : on continue avec 2 LEDs, toute la logique se teste pareil.
- Ports Grove en 5 V : pont diviseur sur l'ultrason.
- La carte ne boote plus à cause de D3 ou RX : plan B du câblage.
- Retard : on sacrifie d'abord les finitions de la page, jamais les sécurités.
