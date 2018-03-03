/*
 * Programme exemple pour lire la luminosité en continu en mode basse résolution 
 * pour un NodeMCU Lolin ou Wemos D1 mini
 */

#include <Wire.h>  // ajout de librairie de gestion I2C
#include <bh1750.h>  // ajout de la librairie de gestion du capteur BH1750

bh1750 mesure_lumiere;  // création d'une instance

// déclaration des broches utilisées sur Wemos D1 mini
int sda = D2;  // GPIO4
int scl = D1;  // GPIO5

void setup()
{
  Serial.begin(115200);
  mesure_lumiere.begin(0x23, sda, scl);  // initialise la librairie BH1750 pour une utilisation Wemos D1 mini
  // adresse 0x23 si broche ADD non branchée ou relié à 5v ou 3.3v
  // adresse 0x5c si broche ADD branchée sur Ov
}

void loop() 
{
  uint16_t lux = mesure_lumiere.lecture_lumiere(MODE_CONTINU_BASSE_RESOLUTION);  // effectue une mesure
  Serial.print("Luminosité : ");
  Serial.print(lux);
  Serial.println(" Lux");
  delay(1000);
}
