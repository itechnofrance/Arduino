/*
* Librairie pour le capteur de température et d'humidité SHT21
* Tester sur NodeMCU Lolin, Wemos D1 Mini et Arduino Nano
* 
* auteur : iTechnoFrance
*/ 
#ifndef sht21_h
#define sht21_h

#include <Arduino.h>
#include "Wire.h"  // ajout du support I2C

class sht21
{
	public:
		uint8_t SHT21_SERIAL_NUMBER[8];
		#if defined(__AVR__)
			// Architecture processeur AVR (Arduino Nano, Uno, Pro etc)
			// initialisation de la librairie
			void begin(int sda = 0xff, int scl = 0xff); 
		#endif
		#if defined(ESP8266)
			// NodeMCU Lolin, Wemos D1 mini etc
			// initialisation de la librairie
			void begin(int sda, int scl); 
		#endif
		void lecture_serial_number();
		float lecture_temperature();
		float lecture_humidite();
		bool etat_alimentation();
		uint8_t lecture_resolution();
		void configure_resolution(uint8_t resolution_mesure);
		
	private:
		void reset();
		uint8_t calcul_crc(uint8_t donnees[], uint8_t nbr_octets);
};
    
#endif
