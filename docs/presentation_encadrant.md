# Voiture RC pilotée par téléphone : fiche de validation

OSMANE Johan, SERGEEV Leonid (3AE FISA)

**L'idée :** une voiture RC (moteur 30 W, servo de direction) pilotée depuis le navigateur du téléphone, sans appli. En plus : aide au recul avec feux, bip qui accélère et arrêt automatique sous un seuil réglable.

**Cas d'utilisation :**
- Conducteur : se connecter, avancer/reculer, diriger, phares, klaxon, voir la distance, régler le seuil.
- Automatique : assister le recul, arrêter la voiture si la liaison est perdue (500 ms) ou si le capteur est KO.

**Architecture :**
- Bibliothèque `PeripheriquesIoT` : `Peripherique` → `Capteur` / `Actionneur` → `Led`, `Buzzer`, `Servomoteur`, `CapteurUltrason`, `MoteurCC` → `MoteurBTS7960`.
- Application : `Application` (template), `Voiture`, `ServeurPilotage`, `Protocole`, `Config.h`.

**Exigences C++ :**
- Héritage : la hiérarchie au-dessus.
- Opérateur : `Serial << moteur` pour l'inventaire et les tests.
- STL : `vector` des périphériques, `map` distance → bip, `nth_element` pour le filtre médian.
- Exceptions : `ErreurBroche` (moteur bloqué), `ErreurMesure` (recul interdit), `ErreurProtocole` (trame ignorée).

**Choix techniques :**
- Mode AP : on ne dépend pas du réseau de l'INSA, adresse fixe `192.168.4.1`.
- WebSocket : trames légères, l'ESP envoie la distance tout seul, déconnexion détectée tout de suite.
- Sécurités : chien de garde, rampe, arrêt avant inversion, pont coupé au boot.
- Pas de `delay()`, une seule bibliothèque externe.

**Brochage :** D1/D2 RPWM/LPWM, D8 EN, D5 servo, D6 ultrason, D7 buzzer, D3 phares, RX feux de recul, D4 LED d'état.

**Planning :** S1 composants du kit, S2 moteur et premier pilotage, S3 appli complète, S4 intégration et rapport.

**Nos questions :**
1. La hiérarchie et la rampe dans la bibliothèque, ça vous va ?
2. `operator<<`, ça suffit pour l'exigence opérateur ?
3. Le mode AP est OK pour la démo ?
4. Une consigne pour tester un moteur de 30 W en salle ?
