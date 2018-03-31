/*
 * Programme exemple pour lire la température et l'humidité du capteur SHT21
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
}

void loop() 
{
  float temperature = SHT21.lecture_temperature();  // récupère la température
  float humidite = SHT21.lecture_humidite();  // récupère l'humidité relative
  if (temperature == -1) { Serial.println("Erreur CRC temperature"); }
  else
  {
	Serial.print("Temperature : ");
	Serial.println(temperature);
  }
  if (humidite == -1) { Serial.println("Erreur CRC humidite"); }
  else
  {
    Serial.print("Humidite : ");
    Serial.println(humidite);
  }
  delay(1000);
}
