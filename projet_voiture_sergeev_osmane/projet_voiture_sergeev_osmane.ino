/*********************************************************************
 * @file  projet_voiture_sergeev_osmane.ino
 * @author L. Sergeev, J. Osmane
 * @brief Point d'entrée Arduino : délègue tout à la classe Application
 *
 * Compilation : carte « NodeMCU 1.0 (ESP-12E Module) »,
 * Outils > C++ Exceptions > Enabled (voir README.md).
 *********************************************************************/
#include "Application.h"

Application myApplication;

void setup()
{
  // put your setup code here, to run once:
  myApplication.init();
}

void loop()
{
  // put your main code here, to run repeatedly:
  myApplication.run();
}
