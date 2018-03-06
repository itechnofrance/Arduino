/*
* Librairie pour le capteur de luminosité BH1750FVI
* Tester sur NodeMCU Lolin, Wemos D1 Mini et Arduino Nano
* auteur : iTechnoFrance
*/ 
#ifndef bh1750_h
#define bh1750_h
    
#include <Arduino.h>
#include "Wire.h"  // ajout du support I2C

// Mesure continue, résolution 1 Lux, temps de mesure 120ms
#define MODE_CONTINU_HAUTE_RESOLUTION  0x10
// Mesure continue, résolution 0.5 Lux, temps de mesure 120ms
#define MODE_2_CONTINU_HAUTE_RESOLUTION 0x11
// Mesure continue, résolution 4 Lux, temps de mesure 16ms
#define MODE_CONTINU_BASSE_RESOLUTION 0x13
// 1 mesure puis passe en veille, résolution 1 Lux, temps de mesure 120ms
#define MODE_UNE_MESURE_HAUTE_RESOLUTION 0x20
// 1 mesure puis passe en veille, résolution 0.5 Lux, temps de mesure 120ms
#define MODE_2_UNE_MESURE_HAUTE_RESOLUTION 0x21
// 1 mesure puis passe en veille, résolution 4 Lux, temps de mesure 16ms
#define MODE_UNE_MESURE_BASSE_RESOLUTION 0x23   

class bh1750
{
	public:
		#if defined(__AVR__)
			// Architecture processeur AVR (Arduino Nano, Uno, Pro etc)
			// initialisation de la librairie
			void begin(uint8_t adresse, int sda = 0xff, int scl = 0xff); 
		#endif
		#if defined(ESP8266)
			// NodeMCU Lolin, Wemos D1 mini etc
			// initialisation de la librairie
			void begin(uint8_t adresse, int sda, int scl); 
		#endif
		uint16_t lecture_lumiere(uint8_t adresse, byte mode_mesure);

	private:
		void reset();
};
    
#endif
