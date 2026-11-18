# Compile l'application et tous les exemples de la bibliotheque avec arduino-cli,
# en affichant tous les avertissements. Echoue si un fichier du projet produit
# un warning (ceux du core ESP8266 ne sont pas comptes).
#
# Usage (PowerShell) : powershell -ExecutionPolicy Bypass -File outils\compiler.ps1

$Racine = Split-Path -Parent $PSScriptRoot
$Fqbn   = "esp8266:esp8266:nodemcuv2:exception=enabled"
$Biblio = Join-Path $Racine "libraries\PeripheriquesIoT"
$Appli  = Join-Path $Racine "projet_voiture_sergeev_osmane"

# Recherche d'arduino-cli : variable, PATH, puis version fournie avec l'IDE Arduino 2.
$Cli = $env:ARDUINO_CLI
if (-not $Cli) {
    $commande = Get-Command arduino-cli -ErrorAction SilentlyContinue
    if ($commande) { $Cli = $commande.Source }
    else { $Cli = Join-Path $env:LOCALAPPDATA "Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe" }
}
if (-not (Test-Path $Cli)) { Write-Error "arduino-cli introuvable : l'installer ou definir ARDUINO_CLI"; exit 2 }

$croquis = @($Appli) + (Get-ChildItem -Directory (Join-Path $Biblio "examples") | ForEach-Object { $_.FullName })
$echec = $false
foreach ($c in $croquis) {
    $nom = Split-Path -Leaf $c
    $sortie = & $Cli compile --fqbn $Fqbn --library $Biblio --warnings all $c 2>&1 | Out-String
    $code = $LASTEXITCODE
    $nosWarnings = $sortie -split "`n" | Where-Object { $_ -match "(projet_voiture_sergeev_osmane|PeripheriquesIoT)[^ ]*:\d+:\d+: warning" }
    if ($code -ne 0) {
        Write-Output "ECHEC  $nom"
        $sortie -split "`n" | Where-Object { $_ -match "error" } | Select-Object -First 20
        $echec = $true
    } elseif ($nosWarnings) {
        Write-Output "WARN   $nom"
        $nosWarnings
        $echec = $true
    } else {
        Write-Output "OK     $nom"
    }
}
if ($echec) { exit 1 } else { exit 0 }
