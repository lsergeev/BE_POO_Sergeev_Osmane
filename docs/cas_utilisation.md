# Cas d'utilisation et scénarios

## 1. Acteurs :

- **Conducteur** : pilote depuis son téléphone.
- **Environnement** : les obstacles derrière la voiture.
- **Chien de garde** : déclenche les sécurités (interne, mais on le dessine pour que ce soit clair).

## 2. Diagramme :

```mermaid
flowchart LR
    C(["👤 Conducteur"])
    E(["🧱 Environnement<br/>(obstacle)"])
    H(["⏱ Chien de garde"])

    subgraph S["Système : Voiture RC (ESP8266)"]
        UC1(["UC1 Se connecter à la voiture"])
        UC2(["UC2 Avancer / reculer<br/>(vitesse proportionnelle)"])
        UC3(["UC3 Diriger"])
        UC4(["UC4 Allumer / éteindre les phares"])
        UC5(["UC5 Klaxonner"])
        UC6(["UC6 Suivre la distance arrière<br/>et l'état"])
        UC7(["UC7 Régler le seuil d'arrêt en recul"])
        UC8(["UC8 Assister le recul<br/>(feux, bip, blocage)"])
        UC9(["UC9 Mettre en sécurité<br/>(liaison perdue, capteur KO)"])
    end

    C --- UC1 & UC2 & UC3 & UC4 & UC5 & UC6 & UC7
    UC2 -. "«include» si v < 0" .-> UC8
    E --- UC8
    H --- UC9
    UC2 -. "«extend»" .-> UC9
```

## 3. Les cas :

| UC | Déclencheur | Résultat |
|---|---|---|
| UC1 Se connecter | Wi-Fi `VoitureRC` puis `192.168.4.1` | La page s'affiche, voyant vert. Un seul conducteur à la fois. |
| UC2 Avancer / reculer | Pédale verticale | Vitesse de −100 à +100 %, avec rampe et arrêt avant de changer de sens |
| UC3 Diriger | Volant horizontal | Les roues suivent, retour au neutre quand on lâche |
| UC4 Phares | Bouton | Les 2 LEDs avant s'allument ou s'éteignent |
| UC5 Klaxon | Bouton | Son de 400 ms, prioritaire sur le bip de recul |
| UC6 Suivre l'état | Automatique, 5 fois par seconde | Distance, vitesse, mode, état du capteur |
| UC7 Régler le seuil | Curseur | Seuil d'arrêt entre 10 et 100 cm (20 par défaut) |
| UC8 Assister le recul | Vitesse négative | Feux de recul, bip de plus en plus rapide, arrêt sous le seuil |
| UC9 Mise en sécurité | 500 ms sans commande, déconnexion ou capteur KO | Moteur arrêté, son coupé. Capteur KO : plus de recul. |

## 4. Scénarios :

### S1 : Démarrage et connexion
1. On allume : pont désactivé (GPIO15 à 0 au boot), roues droites, LED d'état qui clignote lentement.
2. L'inventaire s'affiche sur le port série (`Serial << ...`).
3. Le téléphone rejoint `VoitureRC` et ouvre `http://192.168.4.1`.
4. La page ouvre le WebSocket, voyant vert.
5. La page envoie `C 0 0` toutes les 100 ms, l'ESP renvoie l'état toutes les 200 ms.
6. Variante : une broche fausse dans `Config.h` lève une `ErreurBroche`. Le moteur reste bloqué et la LED clignote très vite.

### S2 : Conduite avec phares
1. Bouton « Phares » : `P 1`, les phares s'allument.
2. Pédale à 70 % : `C 70 0`, la vitesse monte en rampe.
3. On tourne à gauche : `C 70 -50`.
4. Klaxon : `K`, 400 ms.
5. On lâche : `C 0 0`, la voiture ralentit et les roues reviennent droites.
6. Variante : on verrouille le téléphone, plus rien n'arrive. Après 500 ms, l'ESP arrête la voiture.

### S3 : Marche arrière avec obstacle
1. Obstacle à 1,20 m derrière. Pédale à −50 % : `C -50 0`.
2. La voiture recule en rampe, les feux de recul s'allument.
3. Le bip accélère : 500 ms à 80 cm, 250 ms à 50 cm, 120 ms à 30 cm (table `std::map`).
4. Sous 60 cm, le recul est limité à 30 %.
5. À 20 cm : arrêt immédiat, bip continu, la page affiche BLOQUÉ. Les `C -x` sont ignorés.
6. Pédale en avant : la voiture repart. Au-delà de 25 cm le blocage saute, et les feux et le bip s'arrêtent.
7. Variante : capteur débranché, `ErreurMesure`. Recul interdit, la page affiche « capteur KO ».
