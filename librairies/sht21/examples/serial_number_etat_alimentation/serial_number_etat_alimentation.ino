/*
 * Programme exemple pour lire le numéro de série du capteur SHT21 et l'état de l'alimentation
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
  SHT21.lecture_serial_number();  // récupère le numéro de série
  Serial.print("Numero de serie : ");
  for (int i=0; i<8; i++) { Serial.print(SHT21.SHT21_SERIAL_NUMBER[i],HEX); }
  Serial.println();
  Serial.print("Etat alimentation : ");
  if (SHT21.etat_alimentation()) { Serial.print("OK. Alimentation > 2.25v"); }
  else { Serial.print("NOK. Alimentation < 2.25v"); }
  Serial.println(); 
  delay(1000);
}