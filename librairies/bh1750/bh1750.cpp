/*
*	Librairie qui gère le capteur de luminosité BH1750FVI
*	Tester sur NodeMCU Lolin, Wemos D1 Mini et Arduino Nano
*	auteur : iTechnoFrance
*
*/

#include "bh1750.h" 

#define BH1750_POWER_DOWN 0x00  // Mise en veille

#define BH1750_POWER_ON 0x01  // Attente pour une commande

#define BH1750_RESET 0x07  // Mise à 0 du registre de données seulement en mode POWER_ON

uint8_t BH1750_I2CADDR;	 // adresse I2C du capteur BH1750	
// adresse 0x23 si broche ADD non branchée ou relié à 5v ou 3.3v
// adresse 0x5c si broche ADD branchée sur Ov

void bh1750::begin(uint8_t adresse, int sda, int scl)
{
	BH1750_I2CADDR = adresse;
	#if defined(__AVR__)
		// Architecture processeur AVR (Arduino Nano, Uno, Pro etc)
		Wire.begin();  // Arduino (sda = A5, scl = A4)
	#endif
	#if defined(ESP8266)
		// NodeMCU Lolin, Wemos D1 mini
		Wire.begin(sda, scl);  // NodeMCU, Wemos D1 mini (sda = D2, scl = D1)
	#endif
	reset();
}

void bh1750::reset()
{
	Wire.beginTransmission(BH1750_I2CADDR);
	Wire.write((uint8_t)BH1750_POWER_ON);
	Wire.endTransmission();
	delay(10);
	Wire.beginTransmission(BH1750_I2CADDR);
	Wire.write((uint8_t)BH1750_RESET);
	Wire.endTransmission();
	delay(10);
}

uint16_t bh1750::lecture_lumiere(byte mode_mesure)
{
	uint8_t BH1750_MODE = mode_mesure;
	uint16_t luminosite = 0;
	switch (BH1750_MODE)  // vérification du mode à configurer
	{
		case MODE_CONTINU_HAUTE_RESOLUTION:
			Wire.beginTransmission(BH1750_I2CADDR);
			Wire.write((uint8_t)BH1750_MODE);
			Wire.endTransmission();
			break;
		case MODE_2_CONTINU_HAUTE_RESOLUTION:
			Wire.beginTransmission(BH1750_I2CADDR);
			Wire.write((uint8_t)BH1750_MODE);
			Wire.endTransmission();
			break;
		case MODE_CONTINU_BASSE_RESOLUTION:
			Wire.beginTransmission(BH1750_I2CADDR);
			Wire.write((uint8_t)BH1750_MODE);
			Wire.endTransmission();
			break;
		case MODE_UNE_MESURE_HAUTE_RESOLUTION:
			Wire.beginTransmission(BH1750_I2CADDR);
			Wire.write((uint8_t)BH1750_MODE);
			Wire.endTransmission();
			delay(120);   // attente 120ms
			break;
		case MODE_2_UNE_MESURE_HAUTE_RESOLUTION:
			Wire.beginTransmission(BH1750_I2CADDR);
			Wire.write((uint8_t)BH1750_MODE);
			Wire.endTransmission();
			delay(120);   // attente 120ms
			break;
		case MODE_UNE_MESURE_BASSE_RESOLUTION:
			Wire.beginTransmission(BH1750_I2CADDR);
			Wire.write((uint8_t)BH1750_MODE);
			Wire.endTransmission();
			delay(16);   // attente 16ms
			break;
		default:
            Serial.println("Erreur : mode invalide");
			break;
	}
	Wire.requestFrom(BH1750_I2CADDR, (uint8_t)2);
	if (Wire.available() == 2) 
	{
		luminosite = Wire.read();  // lecture octet haut
		luminosite <<= 8;
		luminosite |= Wire.read(); // lecture octet bas
		luminosite /= 1.2;  // transformation en Lux
	}
	return(luminosite);
}


