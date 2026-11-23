/*********************************************************************
 * @file  PageWeb.h
 * @author L. Sergeev, J. Osmane
 * @brief Page de pilotage servie par l'ESP (HTML + CSS + JS, en flash)
 *
 * Première version : deux curseurs (vitesse et direction) qui reviennent
 * à 0 quand on les lâche, et l'état renvoyé par l'ESP. La période
 * d'envoi (100 ms) doit rester égale à config::reseau::PERIODE_COMMANDE_MS.
 *********************************************************************/
#ifndef PAGE_WEB_H_
#define PAGE_WEB_H_

#include <Arduino.h>

/** @brief Contenu de la page, stocké en flash (PROGMEM) */
static const char PAGE_WEB[] PROGMEM = R"PAGE(<!doctype html>
<html lang="fr">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>Voiture RC</title>
<style>
  body { font-family: sans-serif; margin: 0; padding: 16px; background: #1d2327; color: #eee;
         user-select: none; touch-action: none; }
  h1 { font-size: 20px; margin: 0 0 8px; }
  #lien { display: inline-block; width: 12px; height: 12px; border-radius: 50%; background: #c33; }
  #lien.ok { background: #3c3; }
  .zone { display: flex; justify-content: space-around; align-items: center; margin-top: 24px; }
  .bloc { text-align: center; }
  input[type=range] { width: 200px; height: 40px; }
  #vitesse { writing-mode: vertical-lr; direction: rtl; width: 40px; height: 200px; }
  #etat { margin-top: 24px; font-family: monospace; font-size: 15px; white-space: pre; }
</style>
</head>
<body>
<h1><span id="lien"></span> Voiture RC</h1>
<div class="zone">
  <div class="bloc">Vitesse<br><input id="vitesse" type="range" min="-100" max="100" value="0"></div>
  <div class="bloc">Direction<br><input id="direction" type="range" min="-100" max="100" value="0"></div>
</div>
<div id="etat">en attente de la voiture...</div>
<script>
const PERIODE_COMMANDE_MS = 100;  // = config::reseau::PERIODE_COMMANDE_MS
const vitesse = document.getElementById('vitesse');
const direction = document.getElementById('direction');
const lien = document.getElementById('lien');
const etat = document.getElementById('etat');
let ws = null;

function connecter() {
  ws = new WebSocket('ws://' + location.hostname + ':81/');
  ws.onopen = () => lien.classList.add('ok');
  ws.onclose = () => { lien.classList.remove('ok'); setTimeout(connecter, 1000); };
  ws.onmessage = (m) => {
    const e = JSON.parse(m.data);
    if (e.t === 'err') { etat.textContent = 'erreur : ' + e.msg; return; }
    etat.textContent = 'mode     ' + e.mode + '\nvitesse  ' + e.v + ' %  (consigne ' + e.c + ')'
                     + '\ndirection ' + e.dir + ' %\ndistance ' + (e.dist < 0 ? '?' : e.dist + ' cm');
  };
}

// Les curseurs reviennent au neutre quand on les lâche
for (const c of [vitesse, direction]) {
  c.addEventListener('pointerup', () => { c.value = 0; });
  c.addEventListener('touchend', () => { c.value = 0; });
}

// Envoi en continu : sert aussi de battement de coeur pour le chien de garde
setInterval(() => {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send('C ' + vitesse.value + ' ' + direction.value);
  }
}, PERIODE_COMMANDE_MS);

// Téléphone verrouillé ou onglet caché : on arrête tout
document.addEventListener('visibilitychange', () => {
  if (document.hidden && ws && ws.readyState === WebSocket.OPEN) {
    ws.send('C 0 0');
    ws.close();
  }
});

connecter();
</script>
</body>
</html>
)PAGE";

#endif
