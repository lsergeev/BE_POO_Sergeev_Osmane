# Journal de projet

Ce qu'on a fait à chaque séance, ce qui a coincé et comment on l'a réglé. Ça servira pour le rapport.

On ne va pas se mentir : on n'est pas des pros de l'ESP8266. On avance par petits bouts, on teste sur la carte et on corrige.

## Étape 0 : architecture

- On a lu le sujet et le template (un sketch `project_name/` avec une classe `Application`).
- On a séparé le code en deux : la bibliothèque `PeripheriquesIoT` et l'appli.
- On a écrit la traçabilité, les cas d'utilisation, les diagrammes, le brochage, le protocole et le planning.

À vérifier : la tension des ports Grove (pas dans la doc), le fil RX du port UART, et R_EN du BTS7960.

## Séance 1 : outils et composants du kit

**Fait :**
- Sketch renommé en `projet_voiture_sergeev_osmane/` (l'IDE veut le même nom pour le dossier et le `.ino`).
- Core esp8266 3.1.2 installé, scripts de compilation qui plantent au moindre warning.
- `Config.h` avec toutes les constantes.
- Bibliothèque : `Peripherique`, `Capteur`, `Actionneur`, `Led`, `Buzzer`, `Servomoteur`, `CapteurUltrason`, `FiltreMedian` et les exceptions.
- 4 programmes de test.
- Résultat : tout compile sans warning (36 % de RAM, 27 % de flash).

**Problèmes :**

*Exceptions désactivées par défaut.* Si on oublie l'option dans l'IDE, l'erreur est incompréhensible. On a mis un `#error` qui dit quelle option cocher. Testé, ça marche.

*Exception avant `setup()`.* Nos objets sont globaux, donc créés avant `setup()`. Une exception levée à ce moment-là, personne ne peut l'attraper. Du coup, les constructeurs ne vérifient rien : tout se fait dans `begin()`.

*`pulseIn` bloque.* Jusqu'à 30 ms, pendant lesquelles le serveur web ne répond plus. On mesure l'écho par interruption à la place. Piège : il faut couper l'interruption pendant qu'on envoie l'impulsion, sinon on prend notre propre signal pour un écho.

*Pas d'obstacle ou capteur mort ?* Dans les deux cas, on n'a pas de distance. La différence : sans obstacle il y a quand même un écho (très long), alors qu'avec un capteur débranché il n'y a rien. Après 5 échecs, on lève `ErreurMesure`.

*Le bip qui grésille.* `bipPeriodique()` est appelé à chaque tour de boucle. Si ça relance le bip à chaque fois, on n'entend qu'un grésillement. Donc on ne fait rien si la période n'a pas changé.

**Reste à faire sur la carte :** mesures au multimètre, distance à la règle, calibration du servo.

## Entre les séances 1 et 2 : moteur et protocole

**Fait :**
- `MoteurCC` : la rampe (200 %/s pour accélérer, 400 %/s pour freiner) et la pause de 300 ms à l'arrêt avant de changer de sens.
- `MoteurBTS7960` : juste le PWM sur RPWM/LPWM et la broche EN. La bibliothèque est complète.
- `TestMoteur` : avant, arrière direct (pour voir la pause), arrêt, puis arrêt immédiat. Ça marche avec 2 LEDs sur D1 et D2, pas besoin du vrai moteur. La vitesse sort au format du traceur série.
- `Protocole` : décodage des messages de la page (`C`, `P`, `K`, `S`) et état en JSON. Le serveur Wi-Fi n'est pas encore fait.
- Tout compile sans warning (6 programmes).

**Problèmes :**

*Inverser le sens d'un coup.* Si la consigne passe de +60 à -60, il ne faut pas que la vitesse traverse 0 sans s'arrêter. Du coup, on vise d'abord 0, et une fois à l'arrêt on attend 300 ms avant de repartir. Si la voiture était déjà arrêtée depuis longtemps, pas de pause.

*`analogWriteFreq` est globale.* Sur l'ESP8266, la fréquence PWM est la même pour toutes les sorties `analogWrite`. Ça ne gêne pas le servo ni le buzzer (ils ont leur propre générateur), mais c'est noté dans le code.

*Messages tordus.* `C 50-20`, `K 1` ou `C 101 0` doivent être refusés. On a testé une vingtaine de messages, bons et mauvais, et chaque mauvais lève une `ErreurProtocole` avec la raison. Pour le JSON d'erreur, on remplace les guillemets dans la raison, sinon la page ne pourrait pas le lire.

**Reste à faire :** serveur Wi-Fi + WebSocket, première page, et brancher le tout dans `Application`.
