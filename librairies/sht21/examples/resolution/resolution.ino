/*
 * Programme exemple pour lire et modifier la résolution du capteur SHT21
 * pour un NodeMCU Lolin ou Wemos D1 mini
 */

#include <Wire.h>  // ajout de librairie de gestion I2C
#include <sht21.h>  // ajout de la librairie de gestion du capteur SHT21

sht21 SHT21;  // création d'une instance

// déclaration des broches utilisées sur NodeMCU Lolin
int sda = D1;  // GPIO5
int scl = D2;  // GPIO4

void setup()
{
  Serial.begin(115200);
  SHT21.begin(sda, scl);  // initialise la librairie SHT21 pour une utilisation NodeMCU Lolin
  // résolution :
  //				0x00 --> Humidité : 12 bits, Température : 14 bits
  //				0x01 --> Humidité :  8 bits, Température : 12 bits
  //				0x02 --> Humidité : 10 bits, Température : 13 bits
  //				0x03 --> Humidité : 11 bits, Température : 11 bits
  SHT21.configure_resolution(3);
}

void loop() 
{
  uint8_t resolution = SHT21.lecture_resolution();
  if (resolution == 0) { Serial.println("Resolution Humidite : 12 bits, Temperature : 14 bits"); }
  if (resolution == 1) { Serial.println("Resolution Humidite : 8 bits, Temperature : 12 bits"); }
  if (resolution == 2) { Serial.println("Resolution Humidite : 10 bits, Temperature : 13 bits"); }
  if (resolution == 3) { Serial.println("Resolution Humidite : 11 bits, Temperature : 11 bits"); }
  delay(1000);
}