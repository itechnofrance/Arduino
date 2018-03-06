/*
 * Programme exemple pour lire la luminosité en continu en mode haute résolution 
 * pour un Arduino Nano
 */

#include <Wire.h>  // ajout de librairie de gestion I2C
#include <bh1750.h>  // ajout de la librairie de gestion du capteur BH1750

bh1750 mesure_lumiere;  // création d'une instance

void setup()
{
  Serial.begin(115200);
  mesure_lumiere.begin(0x23);  // initialise la librairie BH1750 pour une utilisation Arduino Nano
  // adresse 0x23 si broche ADD non branchée ou relié à 5v ou 3.3v
  // adresse 0x5c si broche ADD branchée sur Ov
}

void loop() 
{
  uint16_t lux = mesure_lumiere.lecture_lumiere(0x23, MODE_CONTINU_HAUTE_RESOLUTION);  // effectue une mesure
  Serial.print("Luminosité : ");
  Serial.print(lux);
  Serial.println(" Lux");
  delay(1000);
}
