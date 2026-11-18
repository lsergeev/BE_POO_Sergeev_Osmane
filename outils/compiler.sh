#!/usr/bin/env bash
# Compile l'application et tous les exemples de la bibliothèque avec arduino-cli,
# en affichant tous les avertissements. Échoue si un fichier du projet produit
# un warning (ceux du core ESP8266 ne sont pas comptés).
#
# Usage : outils/compiler.sh            (tout compiler)
#         ARDUINO_CLI=/chemin/arduino-cli outils/compiler.sh
set -uo pipefail

RACINE="$(cd "$(dirname "$0")/.." && pwd)"
FQBN="esp8266:esp8266:nodemcuv2:exception=enabled"
BIBLIO="$RACINE/libraries/PeripheriquesIoT"
APPLI="$RACINE/projet_voiture_sergeev_osmane"

# Recherche d'arduino-cli : variable, PATH, puis version fournie avec l'IDE Arduino 2.
CLI="${ARDUINO_CLI:-}"
if [ -z "$CLI" ]; then
  if command -v arduino-cli >/dev/null 2>&1; then
    CLI="arduino-cli"
  elif [ -n "${LOCALAPPDATA:-}" ] && [ -x "$LOCALAPPDATA/Programs/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe" ]; then
    CLI="$LOCALAPPDATA/Programs/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe"
  else
    echo "arduino-cli introuvable : l'installer ou definir ARDUINO_CLI" >&2
    exit 2
  fi
fi

echec=0
for croquis in "$APPLI" "$BIBLIO"/examples/*/; do
  croquis="${croquis%/}"
  nom="$(basename "$croquis")"
  sortie="$("$CLI" compile --fqbn "$FQBN" --library "$BIBLIO" --warnings all "$croquis" 2>&1)"
  code=$?
  # Warnings situés dans nos propres fichiers (application ou bibliothèque)
  nos_warnings="$(echo "$sortie" | grep -E "(projet_voiture_sergeev_osmane|PeripheriquesIoT)[^ ]*:[0-9]+:[0-9]+: warning" || true)"
  if [ $code -ne 0 ]; then
    echo "ECHEC  $nom"
    echo "$sortie" | grep -E "error|Error" | head -20
    echec=1
  elif [ -n "$nos_warnings" ]; then
    echo "WARN   $nom"
    echo "$nos_warnings"
    echec=1
  else
    ram="$(echo "$sortie" | grep "Variables and constants in RAM" | grep -oE "[0-9]+%" | head -1)"
    flash="$(echo "$sortie" | grep "Code in flash" | grep -oE "[0-9]+%" | head -1)"
    taille="RAM ${ram:-?}  flash ${flash:-?}"
    echo "OK     $nom  $taille"
  fi
done
exit $echec
