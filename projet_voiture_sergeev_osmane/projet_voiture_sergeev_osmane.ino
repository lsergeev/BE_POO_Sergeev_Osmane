/*********************************************************************
 * @file  projet_voiture_sergeev_osmane.ino
 * @author L. Sergeev, J. Osmane
 * @brief Point d'entrée Arduino : délègue tout à la classe Application
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
