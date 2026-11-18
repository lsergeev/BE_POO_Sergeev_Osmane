# Brochage, câblage et alimentation

## 1. Les broches à surveiller sur la NodeMCU :

| Broche | GPIO | Piège |
|---|---|---|
| D0 | 16 | Pas de PWM, pas d'interruption, pas de port Grove |
| D3 | 0 | Doit être à 1 au boot, sinon la carte part en mode flash |
| D4 | 2 | Doit être à 1 au boot. C'est aussi la LED bleue de la carte. |
| D8 | 15 | Doit être à 0 au boot |
| RX / TX | 3 / 1 | Port série. TX sert aux traces. |
| | 6 à 11 | Flash interne, interdites |

Le shield Grove a les ports D3, D5, D6, D7, D8, A0, 2 × I2C (D1/D2) et un UART (RX/TX). Pas de port D4. On n'utilise que le fil jaune de chaque port.

## 2. Affectation :

| Fonction | Broche | Pourquoi |
|---|---|---|
| RPWM (avant) | D1 (GPIO5) | PWM, aucune contrainte au boot |
| LPWM (arrière) | D2 (GPIO4) | pareil |
| R_EN + L_EN | D8 (GPIO15) | à 0 pendant le boot, donc le pont reste coupé au démarrage |
| Servo | D5 (GPIO14) | rien de spécial |
| Ultrason | D6 (GPIO12) | il faut une interruption |
| Buzzer | D7 (GPIO13) | surtout pas sur D3, son transistor bloquerait le boot |
| Phares | D3 (GPIO0) | LEDs câblées à l'envers (3V3 → R → LED → D3), donc rien ne tire D3 à 0 au boot |
| Feux de recul | RX (GPIO3) | même montage, et le port série passe en émission seule |
| LED d'état | D4 (GPIO2) | la LED bleue de la carte |

Plan B si RX pose problème : R_EN/L_EN reliées au 3V3, une 10 kΩ vers la masse sur RPWM et LPWM, et les feux de recul sur D8. Il suffit de changer `Config.h`.

## 3. Alimentation :

```mermaid
flowchart LR
    BAT["🔋 Batterie moteur<br/>(ex. 3S Li-ion/LiPo 11,1 V<br/>ou selon le moteur)"] --> INT["Interrupteur"] --> FUS["Fusible<br/>10 A"]
    FUS -- "B+" --> BTS["Module BTS7960<br/>(IBT-2)"]
    BTS -- "M+ / M−" --> MOT["Moteur CC 30 W"]
    FUS --> BUCK["Convertisseur abaisseur<br/>→ 5 V / 3 A"]
    BUCK -- "5 V" --> SERVO["Servo Grove<br/>(fil rouge)"]
    BUCK -- "5 V → VIN" --> MCU["NodeMCU<br/>(régulateur 3,3 V interne)"]
    MCU -- "3V3" --> GROVE["Ports Grove :<br/>ultrason, buzzer"]
    MCU -- "3V3" --> VCCL["BTS7960 VCC logique<br/>(3,3 V, voir ci-dessous)"]
    MCU -- "3V3" --> LEDS["4 LEDs + résistances"]
    GND(("⏚ MASSE COMMUNE")) --- BAT & BTS & BUCK & SERVO & MCU
```

- Batterie à part pour le moteur : 30 W, c'est environ 2,5 A sous 12 V, et beaucoup plus au démarrage.
- Servo sur un 5 V à part, sinon ses pics de courant font redémarrer l'ESP. On ajoute un condensateur de 470 µF à côté.
- Masse commune pour tout le monde, sinon le PWM n'a pas de référence.
- VCC logique du BTS7960 en 3,3 V : alimenté en 5 V, il lui faut environ 3,5 V pour voir un 1, et l'ESP ne sort que 3,3 V.
- Sur table (séances 1 et 2) : tout sur l'USB, et 2 LEDs à la place du moteur.

## 4. Compatibilité 3,3 V :

| Module | Verdict |
|---|---|
| Ultrason Grove v2.0 | OK s'il est alimenté en 3,3 V. En 5 V, l'écho arrive à 5 V et il faut un pont diviseur. |
| Buzzer Grove | OK (3,3 à 5 V) |
| Servo Grove | OK avec un 5 V à part |
| BTS7960 | OK avec la logique en 3,3 V |
| LEDs | ~4 mA chacune, 2 par broche, OK |

Résistances : 330 Ω pour une LED rouge ou jaune, 68 Ω pour une blanche (elle éclaire moins en 3,3 V).

## 5. À mesurer au multimètre (séance 1) :

1. VCC d'un port Grove : on attend 3,3 V.
2. Continuité de chaque port vers sa broche, et quel fil de l'UART est RX.
3. Pas de résistance de rappel vers le 3,3 V sur R_EN du BTS7960.
4. Aucune impulsion sur D1, D2 et D8 pendant le boot.
