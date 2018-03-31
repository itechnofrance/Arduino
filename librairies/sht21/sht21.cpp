/*
* Librairie pour le capteur de température et d'humidité SHT21
* Tester sur NodeMCU Lolin, Wemos D1 Mini et Arduino Nano
* 
* auteur : iTechnoFrance
*/ 

#include "sht21.h"

#define SHT21_I2CADDR 0x40	 // adresse I2C du capteur SHT21
#define SHT21_RESET 0xFE  // commande pour effectuer un reboot 
#define SHT21_TEMPERATURE 0xF3  // commande pour lire la température NO HOLD MASTER MODE
#define SHT21_HUMIDITE 0xF5  // commande pour lire l'humidit2 NO HOLD MASTER MODE
#define SHT21_LECTURE_REGISTRE 0xE7  // commande pour lire le registre utilisateur
#define SHT21_ECRITURE_REGISTRE 0xE6  // commande pour ecrire le registre utilisateur

uint8_t SHT21_SERIAL_NUMBER[8];

void sht21::begin(int sda, int scl)
{
	#if defined(__AVR__)
		// Architecture processeur AVR (Arduino Nano, Uno, Pro etc)
		Wire.begin();  // Arduino (sda = A5, scl = A4)
	#endif
	#if defined(ESP8266)
		// NodeMCU Lolin, Wemos D1 mini
		Wire.begin(sda, scl);  // NodeMCU, Wemos D1 mini définition par défaut (sda = D2, scl = D1)
	#endif
	reset();
}

void sht21::reset()
{
	Wire.beginTransmission(SHT21_I2CADDR);
	Wire.write((uint8_t)SHT21_RESET);
	Wire.endTransmission();
	delay(15);  // delai 15ms pour effectuer le reboot
}

void sht21::lecture_serial_number()
{
	// lecture mémoire 1
	Wire.beginTransmission(SHT21_I2CADDR);
	Wire.write((uint8_t)0xFA);
	Wire.write((uint8_t)0x0F);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)SHT21_I2CADDR, (uint8_t)8);  // 8 octets à lire
	if (Wire.available() == 8) 
	{
		// construction du numéro de série
		SHT21_SERIAL_NUMBER[5] = Wire.read();  // SNB3
		Wire.read();  // CRC SNB3
		SHT21_SERIAL_NUMBER[4] = Wire.read();  // SNB2
		Wire.read();  // CRC SNB2
		SHT21_SERIAL_NUMBER[3] = Wire.read();  // SNB1
		Wire.read();  // CRC SNB1
		SHT21_SERIAL_NUMBER[2] = Wire.read();  // SNB0
		Wire.read();  // CRC SNB0
	}
	// lecture mémoire 2
	Wire.beginTransmission(SHT21_I2CADDR);
	Wire.write((uint8_t)0xFC);
	Wire.write((uint8_t)0xC9);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)SHT21_I2CADDR, (uint8_t)6);  // 6 octets à lire
	if (Wire.available() == 6) 
	{
		// construction du numéro de série
		SHT21_SERIAL_NUMBER[1] = Wire.read();  // SNC1
		SHT21_SERIAL_NUMBER[0] = Wire.read();  // SNC0
		Wire.read();  // CRC SNC1-SNC0
		SHT21_SERIAL_NUMBER[7] = Wire.read();  // SNA1
		SHT21_SERIAL_NUMBER[6] = Wire.read();  // SNA0
		Wire.read();  // CRC SNA1-SNA0
	}
}

float sht21::lecture_temperature()
{
    // la température en Celsius est obtenue avec la formule suivante
    // T = -46.85 + 175.72 * (mesure / 2 puissance 16)
    // ou mesure est la valeur fournie par le capteur
	float sht21_temp;
	uint8_t mesure_temperature[2];
	uint8_t crc;
    Wire.beginTransmission(SHT21_I2CADDR);
	Wire.write((uint8_t)SHT21_TEMPERATURE);
	Wire.endTransmission();
	delay(85);  // delai de 85ms pour la mesure de température avec resolution 14 bits par defaut
	Wire.requestFrom((uint8_t)SHT21_I2CADDR, (uint8_t)3);  // 3 octets à lire
	if (Wire.available() == 3) 
	{
		mesure_temperature[0] = Wire.read();  // lecture température poids fort
		mesure_temperature[1] = Wire.read();  // lecture température poids faible
		crc = Wire.read();  // lecture CRC
		// mesure_température[1] & ~0x0003 : efface les 2 derniers bits, qui indiquent le type de mesure température ou humidité
		sht21_temp = (mesure_temperature[0] << 8) + (mesure_temperature[1] & ~0x0003);
		sht21_temp *= 175.72;
		sht21_temp /= (1 << 16);  // divise par 2 puissance 16
		sht21_temp -= 46.85;
		if (calcul_crc(mesure_temperature, 2) == crc) { return sht21_temp; }
        else { return -1; }
	}
}

float sht21::lecture_humidite()
{
    // l'humidité relative en % est obtenue avec la formule suivante
    // RH = -6 + (125 * (mesure / 2 ^16))
    // ou mesure est la valeur fournie par le capteur
    float sht21_humi;
	uint8_t mesure_humidite[2];
	uint8_t crc;
	Wire.beginTransmission(SHT21_I2CADDR);
	Wire.write((uint8_t)SHT21_HUMIDITE);
	Wire.endTransmission();
	delay(29);  // delai de 29ms pour la mesure d'humidité avec résolution 12 bits par defaut
	Wire.requestFrom((uint8_t)SHT21_I2CADDR, (uint8_t)3);  // 3 octets à lire
	if (Wire.available() == 3) 
	{
		mesure_humidite[0] = Wire.read();  // lecture humidité poids fort
		mesure_humidite[1] = Wire.read();  // lecture humidité poids faible
		crc = Wire.read();  // lecture CRC
		// mesure_humidite[1] & ~0x0003 : efface les 2 derniers bits, qui indiquent le type de mesure température ou humidité
		sht21_humi = (mesure_humidite[0] << 8) + (mesure_humidite[1] & ~0x0003);
		sht21_humi *= 125;
		sht21_humi /= (1 << 16);  // divise par 2 puissance 16
		sht21_humi -= 6;
		if (calcul_crc(mesure_humidite, 2) == crc) { return sht21_humi; }
        else { return -1; }
	}
}

uint8_t sht21::calcul_crc(uint8_t donnees[], uint8_t nbr_octets)
{
	// calcul le CRC
	uint8_t crc = 0;
    uint16_t POLYNOMIAL = 0x131;  // P(x)=x^8+x^5+x^4+1 = 100110001 fournit par la documentation du capteur
    for (uint8_t octet = 0; octet < nbr_octets; octet++)
	{
		crc ^= donnees[octet];  // XOR
		for (uint8_t bit = 8; bit > 0; bit--)
		{
			if (crc & 0x80) { crc = (crc << 1) ^ POLYNOMIAL; }
			else { crc = (crc << 1); }
		}
	}
	return crc;	
}
	
bool sht21::etat_alimentation()
{
    // permet de vérifier si l'alimentation du module est ok : > 2.25v
    Wire.beginTransmission(SHT21_I2CADDR);
	Wire.write((uint8_t)SHT21_LECTURE_REGISTRE);
	Wire.endTransmission();
    Wire.requestFrom((uint8_t)SHT21_I2CADDR, (uint8_t)1);  // 1 octet à lire
	uint8_t registre = Wire.read();  
	registre &= 0x40;  // récupère le bit 6
    if (registre == 0x40) { return(false); }  // alimentation < 2.25v
    else { return(true); }  // alimentation > 2.25v
}

uint8_t sht21::lecture_resolution()
{
	// récupère la résolution de la mesure Humidité et Température
	Wire.beginTransmission(SHT21_I2CADDR);
	Wire.write((uint8_t)SHT21_LECTURE_REGISTRE);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)SHT21_I2CADDR, (uint8_t)1);  // 1 octet à lire
	uint8_t registre = Wire.read(); 
	uint8_t resolution = registre & 0x81;  // récupère le bit 7 et le bit 0
	// résolution :
	//				0x00 --> Humidité : 12 bits, Température : 14 bits
	//				0x01 --> Humidité :  8 bits, Température : 12 bits
	//				0x80 --> Humidité : 10 bits, Température : 13 bits
	//				0x81 --> Humidité : 11 bits, Température : 11 bits
	if (resolution == 0x80) { resolution = 2; }
	if (resolution == 0x81) { resolution = 3; }
	return(resolution);
}
    
void sht21::configure_resolution(uint8_t resolution_mesure)
{
	// configure la résolution de la mesure Humidité et Température
	uint8_t resolution;
	Wire.beginTransmission(SHT21_I2CADDR);
	Wire.write((uint8_t)SHT21_LECTURE_REGISTRE);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)SHT21_I2CADDR, (uint8_t)1);  // 1 octet à lire
	uint8_t registre = Wire.read(); 
	if (resolution_mesure == 0)
	{
		// résolution Humidité : 12 bits Température : 14 bits
		resolution = registre & 0x7E; // bit 7 et bit 0 à 0
        Wire.beginTransmission(SHT21_I2CADDR);
		Wire.write((uint8_t)SHT21_ECRITURE_REGISTRE);
		Wire.write(resolution);
		Wire.endTransmission();
	}
	if (resolution_mesure == 1)
	{
		// résolution Humidité : 8 bits Température : 12 bits
		resolution = registre & 0x7F | 0x01;  // bit 7 à 0 et bit 0 à 1
        Wire.beginTransmission(SHT21_I2CADDR);
		Wire.write((uint8_t)SHT21_ECRITURE_REGISTRE);
		Wire.write(resolution);
		Wire.endTransmission();
	}
	if (resolution_mesure == 2)
	{
		// résolution Humidité : 10 bits Température : 13 bits
		resolution = registre & 0xFE | 0x80;  // bit 7 à 1 et bit 0 à 0
        Wire.beginTransmission(SHT21_I2CADDR);
		Wire.write((uint8_t)SHT21_ECRITURE_REGISTRE);
		Wire.write(resolution);
		Wire.endTransmission();
	}
	if (resolution_mesure == 3)
	{
		// résolution Humidité : 11 bits Température : 11 bits
		resolution = registre | 0x81;  // bit 7 à 1 et bit 0 à 1
        Wire.beginTransmission(SHT21_I2CADDR);
		Wire.write((uint8_t)SHT21_ECRITURE_REGISTRE);
		Wire.write(resolution);
		Wire.endTransmission();
	}
}