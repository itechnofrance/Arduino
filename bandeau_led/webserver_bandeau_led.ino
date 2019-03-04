/*
 * Programme qui permet de gérer une bande de LED WS2812 via un navigateur WEB
 * Attention : avant de charger ce programme, il est nécessaire d'effacer l'EEPROM
 *             pour cela utiliser le programme eeprom_clear fournit avec la librairie ESP8266
 * 
 * Matériel :
 *            NodeMCU Lolin V3 ou WEMOS D1 Mini
 *            Bandeau de LED WS2812
 *            Capteur température LM35DZ
 *            
 */

// Ajout librairie EEPROM
#include <EEPROM.h>

// Ajout librairie gestion WIFI
#include <ESP8266WiFi.h>

// Ajout librairie Web Server
#include <ESP8266WebServer.h>
// Création d'une instance serveur
ESP8266WebServer server(80); // port d'écoute 80

// Ajout librairie UDP et OTA pour téléverser un programme via WiFi
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Ajoute la librairie de gestion de nom DNS
#include <ESP8266mDNS.h>

// Ajout librairie Adafruit Neopixel
#include <Adafruit_NeoPixel.h>

// Pages HTML
#include "index.h"
#include "ambiance.h"
#include "animations.h"
#include "temperature.h"
#include "administration.h"
String page_html_active = "aucune";

// Déclaration WIFI pour la connexion
String ssid = "";  // SSID en mode client
String password = "";  // mot de passe pour la connexion au point d'accès
String ssid_apmode = "Ambiance"; // SSID en mode point d'accès
String password_apmode = "ambiance";  // mot de passe pour le point d'accès
String ssid_eeprom = "";     // SSID contenu dans l'EEPROM
String password_eeprom = ""; // Mot de passe contenu dans l'EEPROM 
byte wifi_erreur; // gestion d'un problème de connexion au point d'accès WiFi

String XML; // variable qui permet les échanges en XML
int sliderValue[4] = {0, 0, 0, 100}; // initialisation des 4 sliders 
byte bouton_status = 1; // gestion des boutons pour les activer ou désactiver
String animation_html = "Aucune"; // indicateur HTML pour l'animation en cours
String mac_address_html = ""; // Mac Address du module WiFi
String ssid_actif_html = ""; // SSID utilisé

// nom du module utilisé pour la MAJ OTA et pour le réseau
// supporté par Windows, Linux, Mac OS, IOS mais pas sur Android
char *nom_ota = "amb-bar"; // accessible via http://amb-bar.local

// Déclaration capteur de température LM35
int tension= analogRead(A0); // entrée analogique A0
float temp_celsius; // contient la température
char string_temp_celsius[4]; // transforme la température en caractère xx.y
float temp_24heures[25]; // tableau qui contient les températures des dernières 24 heures
int mesures_temp =0; // compteur des mesures de température
String graph_temperature = ""; // contient les coordonnées du graphe à afficher
unsigned long compteur = 0; // compteur pour lire la température toutes les 15 mn

// définition pour la librairie Adafruit Neopixel
#define bandeau 0  // Sortie D3 liaison vers le bandeau de LED
#define nbr_leds 121 // nombre de LEDS
boolean affichage = false; // variable utilisée par certaines animations
// Animations suppportées
enum animation { NONE, AMBIANCE_ANIME, VAGUE, ARC_EN_CIEL, FONDU, THEATRE, SCANNER, FEU, VOLET, 
                  HELIPORT, SCINTILLEMENT, HALLOWEEN, FRANCE, METEOR, CYLON };
// direction des animations supportées
enum direction { forward, inverse };

class animations : public Adafruit_NeoPixel
{
  public:
  animation animation_active; // quelle animation est en cours d'exécution
  direction Direction; // direction pour exécuter l'animation
  unsigned long Interval; // milliseconds entre les mises à jour du bandeau
  unsigned long lastUpdate; // dernière mise à jour de la position
  uint32_t Color1, Color2; // couleurs à utiliser
  uint16_t TotalSteps; // le nombre total d'étapes dans l'animation
  uint16_t Index; // étape courante à l'intérieur de l'animation
  boolean led_status = false; // utiliser par certaines animations
  uint8_t Etapes; // utiliser par certaines animations
  byte MeteorSize; // taille du Météor utiliser par l'animation Meteor
  boolean  Debris_aleatoire; // active ou non les débris en mode aléatoire utiliser par l'animation Meteor
     
  void (*OnComplete)(); // rappel à la fin de l'animation

  // Constructeur pour initialiser le bandeau
  animations(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
  :Adafruit_NeoPixel(pixels, pin, type)
  {
    OnComplete = callback;
  }

  // met à jour l'animation
  void Update()
  {
    if((millis() - lastUpdate) > Interval) // il est temps de mettre à jour
    {
      lastUpdate = millis();
      switch(animation_active)
      {
        case NONE:
            no_animation();
            break;
        case AMBIANCE_ANIME:
            ambiance_animeUpdate();
            break;
        case VAGUE:
            vagueUpdate();
            break;
        case ARC_EN_CIEL:
            arc_en_cielUpdate();
            break;
        case FONDU:
            fonduUpdate();
            break;
        case THEATRE:
            theatreUpdate();
            break;
        case SCANNER:
            scannerUpdate();
            break;
        case FEU:
            feuUpdate();
            break;
        case VOLET:
            voletUpdate();
            break;
        case HELIPORT:
            heliportUpdate();
            break;
        case SCINTILLEMENT:
            scintillementUpdate();
            break;
        case HALLOWEEN:
            halloweenUpdate();
            break;
        case FRANCE:
            franceUpdate();
            break;
        case METEOR:
            meteorUpdate();
            break;
        case CYLON:
            cylonUpdate();
            break;
        default:
            break;
      }
    }
  }

  // incrémente l'index et RAZ à la fin
  void Increment()
  {
    if (Direction == forward)
    {
      Index++;
      if (Index >= TotalSteps)
      {
        Index = 0;
        if (OnComplete != NULL) { OnComplete(); } // appelle la fonction en fin d'exécution
      }
    }
    else // Direction == inverse
    {
      --Index;
      if (Index <= 0)
      {
        Index = TotalSteps-1;
        if (OnComplete != NULL) { OnComplete(); } // appelle la fonction en fin d'exécution
      }
    }
  }

  // direction inverse de l'animation
  void Reverse()
  {
    if (Direction == forward)
    {
      Direction = inverse;
      Index = TotalSteps-1;
    }
    else
    {
      Direction = forward;
      Index = 0;
    }
  }

  // aucune animation
  void no_animation()
  {
    animation_active = NONE;
  }
  
  // initialise le mode ambiance animé
  void ambiance_anime(uint32_t color, uint8_t interval, direction dir)
  {
    animation_active = AMBIANCE_ANIME;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color;
    Index = 0;
    Direction = dir;
  }
 
  // Mise à jour du cycle de l'animation ambiance
  void ambiance_animeUpdate()
  {
    setPixelColor(Index, Color1);
    show();
    Increment();
  }
  
  // Initialise le mode vague
  void vague(uint32_t color, uint8_t interval, direction dir = forward)
  {
    animation_active = VAGUE;
    Interval = interval;
    TotalSteps = numPixels()* 2;
    Color1 = color;
    Index = 0;
    Direction = dir;
  }
  
  // Mise à jour du cycle de l'animation vague
  void vagueUpdate()
  {
    for(int i=0; i < TotalSteps / 2; i++)
    {
      setPixelColor(i,((sin(i + Index) * 127 + 128)/255) * Rouge(Color1), ((sin(i + Index) * 127 + 128)/255) * Vert(Color1), ((sin(i + Index) * 127 + 128)/255) * Bleu(Color1));
    }
    show();
    Increment();
    delay(40);
  }

  // Initialise le mode arc en ciel
  void arc_en_ciel(uint8_t interval, direction dir = forward)
  {
    animation_active = ARC_EN_CIEL;
    Interval = interval;
    TotalSteps = 255;
    Index = 0;
    Direction = dir;
  }
    
  // Mise à jour du cycle de l'animation arc en ciel
  void arc_en_cielUpdate()
  {
    for(int i=0; i < numPixels(); i++)
    {
        setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
    }
    show();
    Increment();
  }

  // Initialise l'animation fondu
  void fondu(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = forward)
  {
    animation_active = FONDU;
    Interval = interval;
    TotalSteps = steps;
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
  }
    
  // mise à jour du cycle fondu
  void fonduUpdate()
  {
    uint8_t rouge = ((Rouge(Color1) * (TotalSteps - Index)) + (Rouge(Color2) * Index)) / TotalSteps;
    uint8_t vert = ((Vert(Color1) * (TotalSteps - Index)) + (Vert(Color2) * Index)) / TotalSteps;
    uint8_t bleu= ((Bleu(Color1) * (TotalSteps - Index)) + (Bleu(Color2) * Index)) / TotalSteps;
    ColorSet(Color(rouge, vert, bleu));
    show();
    Increment();
    if (Index == (TotalSteps -1)) { Reverse(); }
  }

  // Initialise l'animation théâtre
  void theatre(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = forward)
  {
    animation_active = THEATRE;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
  }
    
  // mise à jour de l'animation théâtre
  void theatreUpdate()
  {
    for(int i=0; i< numPixels(); i++)
    {
      if ((i + Index) % 3 == 0) { setPixelColor(i, Color1); }
      else { setPixelColor(i, Color2); }
    }
    show();
    Increment();
  }

  // Initialise l'animation scanner
  void scanner(uint32_t color1, uint8_t interval)
  {
    animation_active = SCANNER;
    Interval = interval;
    TotalSteps = (numPixels() - 1) * 2;
    Color1 = color1;
    Index = 0;
  }
 
  // mise à jour de l'animation scanner
  void scannerUpdate()
  { 
    for (int i = 0; i < numPixels(); i++)
    {
      if (i == Index) { setPixelColor(i, Color1); }  // Scan Pixel vers la droite
      else if (i == TotalSteps - Index) { setPixelColor(i, Color1); } // Scan Pixel vers la gauche
           else { setPixelColor(i, DimColor(getPixelColor(i))); }
    }
    show();
    Increment();
  }

  // initialise l'animation feu
  void feu(uint32_t color, uint8_t interval, direction dir = forward)
  {
    animation_active = FEU;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color;
    Index = 0;
    Direction = dir;
  }

  // Mise à jour du cycle de l'animation feu
  void feuUpdate()
  {
    int flicker = random(0,40);
    uint8_t rouge = Rouge(Color1) - flicker;
    uint8_t vert = Vert(Color1) - flicker;
    uint8_t bleu = Bleu(Color1) - flicker;
    if(vert < 0) vert = 0;
    if(rouge < 0) rouge = 0;
    if(bleu < 0) bleu = 0;
    setPixelColor(Index, rouge, vert, bleu);
    show();
    Increment();
  }

  // initialise le mode animation volet
  void volet(uint32_t color, uint8_t interval, direction dir)
  {
    animation_active = VOLET;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color;
    Index = 0;
    Direction = dir;
  }

  // Mise à jour du cycle de l'animation volet
  void voletUpdate()
  {
    setPixelColor(Index, Color1);
    show();
    Increment();
  }

  // initialise l'animation heliport
  void heliport(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = forward)
  {
    animation_active = HELIPORT;
    Interval = interval;
    TotalSteps = int (numPixels() / 2);
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
  }

  // Mise à jour de l'animation heliport
  void heliportUpdate()
  {
    setPixelColor(Index, Color1);
    setPixelColor((TotalSteps*2) - Index -1, Color2);
    show();
    Increment();
  }

  // initialise l'animation scintillement
  void scintillement(uint32_t color1, uint8_t interval, direction dir = forward)
  {
    animation_active = SCINTILLEMENT;
    Interval = interval;
    TotalSteps = int (numPixels() / 5);
    Color1 = color1;
    Index = 0;
    Direction = dir;
  }

  // Mise à jour de l'animation scintillement
  void scintillementUpdate()
  {
    setPixelColor(random(numPixels()), Color1);
    show();
    Increment();
  }

  // initialise l'animation halloween
  void halloween(uint8_t interval, direction dir = forward)
  {
    animation_active = HALLOWEEN;
    Interval = interval;
    TotalSteps = int (numPixels() / 5);
    Index = 0;
    Direction = dir;
  }

  // Mise à jour de l'animation haloween
  void halloweenUpdate()
  {
    setPixelColor(random(numPixels()), 100, 0, 60); // violet
    setPixelColor(random(numPixels()), 255, 65, 0);  // orange
    setPixelColor(random(numPixels()), (0, 0, 0));
    show();
    Increment();
  }

  // initialise l'animation france
  void france(uint8_t interval, uint8_t etapes, direction dir = forward)
  {
    animation_active = FRANCE;
    Interval = interval;
    TotalSteps = int (numPixels() / 3);
    Index = 0;
    Direction = dir;
    Etapes = etapes;
  }

  // Mise à jour de l'animation france
  void franceUpdate()
  {
    if (Etapes == 1)
    {
      setPixelColor(Index, 0, 0, 255); // bleu
      setPixelColor(TotalSteps + Index , 255, 255, 255); // blanc
      setPixelColor((TotalSteps*2) + Index , 255, 0, 0); // rouge
    }
    if (Etapes == 3) { ColorSet(Color(0, 0, 255)); }
    if (Etapes == 4) { ColorSet(Color(255, 255, 255)); }
    if (Etapes == 5) { ColorSet(Color(255, 0, 0)); }
    if (Etapes == 6) 
    { for (int i = 0; i < numPixels(); i = i+3)
      {
        setPixelColor(i, 0, 0, 255);
        setPixelColor(i+1, 255, 255, 255);
        setPixelColor(i+2, 255, 0, 0);
      }
    }
    show();
    Increment();
    if (Etapes == 8) { Etapes = 1; }
  }

  // initialise l'animation meteor
  void meteor(uint32_t color1, byte meteorSize, boolean debris_aleatoire, uint8_t interval, direction dir = forward) 
  {
    animation_active = METEOR;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color1;
    MeteorSize = meteorSize;
    Debris_aleatoire = debris_aleatoire;
    Index = 0;
    Direction = dir;
  }

  // Mise à jour de l'animation meteor
  void meteorUpdate()
  {
    // baisse la luminosité de toutes les LEDs
    for (int j=0; j < numPixels(); j++) { if ((!Debris_aleatoire) || (random(10) > 5)) { fadeToBlack(j, 128); } }
    
    // dessine le météor
    for (int j = 0; j < MeteorSize; j++) { if ((Index-j < numPixels()) && (Index-j >= 0)) { setPixelColor(Index-j, Color1); } }
    show();
    Increment();
  }

  // Initialise le mode cylon
  void cylon(uint32_t color, uint8_t interval, direction dir = forward)
  {
    animation_active = CYLON;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color;
    Index = 0;
    Direction = dir;
  }
  
  // Mise à jour du cycle de l'animation cylon
  void cylonUpdate()
  { if (Direction == forward)
    {
    setPixelColor(Index, Color1);
    setPixelColor(Index + 1, Color1);
    setPixelColor(Index + 2, Color1);
    setPixelColor(Index + 3, Color1);
    setPixelColor(Index + 4, Color1);
    setPixelColor(Index - 1, (0, 0, 0));
    setPixelColor(Index - 2, (0, 0, 0));
    setPixelColor(Index - 3, (0, 0, 0));
    setPixelColor(Index - 4, (0, 0, 0));
    show();
    }
    if (Direction == inverse)
    {
    setPixelColor(Index, Color1);
    setPixelColor(Index - 1, Color1);
    setPixelColor(Index - 2, Color1);
    setPixelColor(Index - 3, Color1);
    setPixelColor(Index - 4, Color1);
    setPixelColor(Index + 1, (0, 0, 0));
    setPixelColor(Index + 2, (0, 0, 0));
    setPixelColor(Index + 3, (0, 0, 0));
    setPixelColor(Index + 4, (0, 0, 0));
    show();
    }
    Increment();
    if (Index == (numPixels() -1)) { Reverse(); }
  }
  
  void fadeToBlack(int ledNo, byte fadeValue) // utiliser par l'animation METEOR
  {
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
    
    oldColor = getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);
    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
    setPixelColor(ledNo, r,g,b);
  }
  
  // les couleurs sont une transition R - V - B - retour vers R
  uint32_t Wheel(byte WheelPos)
  {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) { return Color(255 - WheelPos * 3, 0, WheelPos * 3); }
    else if(WheelPos < 170)
    {
      WheelPos -= 85;
      return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
      WheelPos -= 170;
      return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
  }

  // Met toutes les LEDs à une couleur 
  void ColorSet(uint32_t color)
  {
    for (int i = 0; i < numPixels(); i++) { setPixelColor(i, color); }
    show();
  }
 
  // Renvoi le composant Rouge en une couleur 32-bit 
  uint8_t Rouge(uint32_t color)
  { return (color >> 16) & 0xFF; }
 
  // Renvoi le composant vert en une couleur 32-bit 
  uint8_t Vert(uint32_t color)
  { return (color >> 8) & 0xFF; }
 
  // Renvoi le composant Bleu en une couleur 32-bit 
  uint8_t Bleu(uint32_t color)
  { return color & 0xFF; }

  // Calcul 50% d'obscurcissement de la couleur 
  uint32_t DimColor(uint32_t color)
  {
    // Décale R - V - B d'un bit vers la droite
    uint32_t dimColor = Color(Rouge(color) >> 1, Vert(color) >> 1, Bleu(color) >> 1);
    return dimColor;
  }
};

void bandeauledComplete();
animations bandeauled(nbr_leds, bandeau, NEO_GRB + NEO_KHZ800, &bandeauledComplete);

void setup() 
{
  Serial.begin(115200);
  EEPROM.begin(512);  // initialisation utilisation de l'EEPROM de 512 Octets
  delay(100);
  for (int i = 0; i < 32; ++i) { ssid_eeprom += char(EEPROM.read(i)); } // Récupère le SSID
  for (int i = 32; i < 96; ++i) { password_eeprom += char(EEPROM.read(i)); } // Récupère le mot de passe
  for (int i= 96; i < 99; ++i) { sliderValue[i - 96] = EEPROM.read(i); } // Récupère les valeurs RGB
  wifi_erreur =EEPROM.read(99); // récupère le nombre d'erreur
  if ((ssid_eeprom.length() > 1) && (ssid_eeprom[0] > 0)) // SSID sauvegardé dans la mémoire EEPROM
  { 
    if (wifi_erreur < 4) // 3 erreurs de connexion
    {
      WiFi.mode(WIFI_STA); // configure le WIFI en mode station
      WiFi.disconnect();  // on se déconnecte si on été déjà connecté à un point d'accès WiFi
      WiFi.begin(ssid_eeprom.c_str(), password_eeprom.c_str()); // connexion au point d'accès WiFi
      while (WiFi.waitForConnectResult() != WL_CONNECTED) // on attend la connexion
      {
        wifi_erreur++; // on incrémente le nbr de tentatives en erreur
        EEPROM.write(99, wifi_erreur); // on enregistre le nbr de tentatives dans l'EEPROM
        EEPROM.commit(); // on valide l'écriture
        delay(10000); // on attend 10 secondes
        ESP.restart(); // il y a une erreur de connexion on reboot
      }
      ssid_actif_html = ssid_eeprom.c_str();
    }
    else
    {
      wifi_erreur = 0;
      EEPROM.write(99, wifi_erreur); // on enregistre le nbr de tentatives dans l'EEPROM
      EEPROM.commit(); // on valide l'écriture
      WiFi.mode(WIFI_AP); // configure le WiFi en point d'accès
      ssid_apmode = ssid_apmode + "_" + WiFi.macAddress(); // créer un SSID contenant l'adresse MAC du module
      WiFi.softAP(ssid_apmode.c_str(), password_apmode.c_str(), 10); // configure le module en point d'accès en utilisant le canal 10
      ssid_actif_html = ssid_apmode.c_str();
    }
  }
  else
  {
    WiFi.mode(WIFI_AP); // configure le WiFi en point d'accès
    ssid_apmode = ssid_apmode + "_" + WiFi.macAddress(); // créer un SSID contenant l'adresse MAC du module
    WiFi.softAP(ssid_apmode.c_str(), password_apmode.c_str(), 10); // configure le module en point d'accès en utilisant le canal 10
    ssid_actif_html = ssid_apmode.c_str();
  }
  mac_address_html = WiFi.macAddress().c_str();
  server.on("/", page_html); // page par défaut
  server.on("/ambiance", page_html_ambiance); // page ambiance
  server.on("/animations", page_html_animations); // page ambiance
  server.on("/temperature", page_html_temperature); // page température
  server.on("/administration", page_html_administration); // page administration
  server.on("/xml", handleXML); // Traite la requête XML
  server.on("/set_neopixel", traite_val_neopixel); // traite les sliders pour la bande de LED
  server.onNotFound(page_html_non_trouve); // page non trouvée, on associe la fonction à traiter
  server.begin(); // démarre le serveur WEB
  ArduinoOTA.setHostname(nom_ota); // définit le nom du module pour l'utilisation réseau et OTA
  ArduinoOTA.setPassword((const char *)"1964"); // définit un mot de passe pour la mise à jour
  ArduinoOTA.onStart([]() { bandeauled.no_animation(); bandeauled.ColorSet(couleur(0, 0, 0)); }); // action à effectuer lors du commencement du téléchargement
  ArduinoOTA.onEnd([]() {}); // action à effectuer à la fin du téléchargement
  ArduinoOTA.begin(); // initialise la possibilité de MAJ via le WiFi
  bandeauled.begin(); // démarre l'utilisation de la librairie Neoopixel
  lecture_temperature();
  delay(5000);
  bandeauled.show(); // Eteint toutes les LED
  bandeauled.no_animation();
}

uint32_t couleur_aleatoire()
{
  uint32_t r_aleatoire = random(255) << 16 & 0xFFFFFF; // rouge
  uint32_t v_aleatoire = random(255) << 8 & 0xFFFFFF; // vert
  uint32_t b_aleatoire = random(255) & 0xFFFFFF; // bleu
  uint32_t c_aleatoire = r_aleatoire + v_aleatoire + b_aleatoire;
  return(c_aleatoire);
}

uint32_t couleur(int rouge, int vert, int bleu)
{
  uint32_t r = rouge << 16 & 0xFFFFFF; // rouge
  uint32_t v = vert << 8 & 0xFFFFFF; // vert
  uint32_t b = bleu & 0xFFFFFF; // bleu
  uint32_t c = r + v + b;
  return(c);
}

void bandeauledComplete()
{
  switch(bandeauled.animation_active)
  {
    case NONE:
      bandeauled.no_animation();
      break;
    case AMBIANCE_ANIME:
      bandeauled.Color1 = couleur_aleatoire();
      break;
    case VAGUE:
      bandeauled.Color1 = couleur_aleatoire();
      break;
    case ARC_EN_CIEL:
      bandeauled.Interval = 10; // transition 10ms
      break;
    case FONDU:
      bandeauled.Color1 = (0,0,0);
      bandeauled.Color2 = couleur_aleatoire(); 
      break;
    case THEATRE:
      bandeauled.Color1 = (0,0,0);
      bandeauled.Color2 = couleur_aleatoire(); 
      break;
    case SCANNER:
      bandeauled.Color1 = couleur_aleatoire();
      break;
    case FEU:
      bandeauled.Interval = random(10,50);
      break;
    case VOLET:
      bandeauled.led_status = !bandeauled.led_status;
      if (bandeauled.led_status == true) { bandeauled.Color1 = (0, 0, 0); }
      else { bandeauled.Color1 = couleur_aleatoire(); }
      break;
    case HELIPORT:
      bandeauled.led_status = !bandeauled.led_status;
      if (bandeauled.led_status == true) { bandeauled.Color1 = (0, 0, 0); bandeauled.Color2 = (0, 0, 0); }
      else { bandeauled.Color1 = couleur_aleatoire(); bandeauled.Color2 = couleur_aleatoire(); } 
      break;
    case SCINTILLEMENT:
      bandeauled.led_status = !bandeauled.led_status;
      if (bandeauled.led_status == true) { bandeauled.Color1 = (0, 0, 0); }
      else { bandeauled.Color1 = couleur_aleatoire(); }
      break;
    case HALLOWEEN:
      bandeauled.Interval = 200; // transition 200ms
      break;
    case FRANCE:
      bandeauled.Etapes ++;
      break;
    case METEOR:
      bandeauled.Color1 = couleur_aleatoire();
      break;
    case CYLON:
      bandeauled.Color1 = couleur_aleatoire();
      break;
    default:
      break;
  }
}

void construit_graph_temperature()
{
  float axe_x, axe_y;
  char  string_axe_x[4], string_axe_y[4];
  graph_temperature = "";
  for (int i=0; i<25; i++)
  {
    axe_x = 50.0 + (i * 10);
    axe_y = 270 - (temp_24heures[i] * 5);
    dtostrf(axe_x, 2, 0, string_axe_x);
    dtostrf(axe_y, 2, 0, string_axe_y);
    graph_temperature += String (string_axe_x) + "," + String (string_axe_y) + " ";
  }
}

void lecture_temperature()
{
  int reste;
  int tension= analogRead(A0);
  float millivolts= (tension/1024.0) * 2766; // calibrage 2766 mV et non 3300 mV
  temp_celsius= millivolts/10.0;
  dtostrf(temp_celsius, 2, 1, string_temp_celsius);
  reste = (mesures_temp) % 4; // calcul le modulo pour enregistrer toutes les 4 mesures (1 mesure toutes les 15mn) 
  mesures_temp ++;
  if (reste == 0) 
  { 
    for (int i=0; i<25; i++) { temp_24heures[i] = temp_24heures[i+1]; }
    if (temp_celsius > 50) { temp_24heures[24] = 50; }
    else { temp_24heures[24] = temp_celsius; }
    if (mesures_temp >= 5) { mesures_temp = 1; }
    construit_graph_temperature();
  }
}

void ClearEEprom()
{
  for (int i = 0; i < 99; ++i) { EEPROM.write(i, 0); } 
  // efface les 99 premiers octets de l'EEPROM mais pas l'octet 99 qui contient les erreurs WiFi
  EEPROM.commit();
}

void traite_val_neopixel() 
{
  int sliderCNT = server.arg("cnt").toInt();
  sliderValue[sliderCNT] = server.arg("val").toInt();
  handleXML();
  if ((animation_html == "Aucune") && (bouton_status == 0))
  {
    if (sliderCNT == 3) { bandeauled.setBrightness(sliderValue[3]); }
    bandeauled.ColorSet(couleur(sliderValue[0], sliderValue[1], sliderValue[2]));
  }
}

void handleXML() 
{
  buildXML(); // construit le XML
  server.send(200, "text/xml", XML); // envoie le XML
}

void buildXML() 
{
  XML = "<?xml version='1.0'?>";
  XML += "<xml>";
  if (page_html_active == "ambiance")
  {
    XML += "<slidervalue0>";
    XML += String(sliderValue[0]);
    XML += "</slidervalue0>";
    XML += "<slidervalue1>";
    XML += String(sliderValue[1]);
    XML += "</slidervalue1>";
    XML += "<slidervalue2>";
    XML += String(sliderValue[2]);
    XML += "</slidervalue2>";
    XML += "<slidervalue3>";
    XML += String(sliderValue[3]);
    XML += "</slidervalue3>";
    XML += "<bouton_status>";
    XML += byte(bouton_status);
    XML += "</bouton_status>";
  }
  if (page_html_active == "animation")
  {
    XML += "<animation>";
    XML += String(animation_html);
    XML += "</animation>";
  }
  if (page_html_active == "administration")
  {
    XML += "<macaddress>";
    XML += String(mac_address_html);
    XML += "</macaddress>";
    XML += "<ssid>";
    XML += String(ssid_actif_html);
    XML += "</ssid>";
  }
  if (page_html_active == "temperature")
  {
    XML += "<temp_actuelle>";
    XML += String(string_temp_celsius);
    XML += "</temp_actuelle>";
    XML += "<temp_graphique>";
    XML += String(graph_temperature);
    XML += "</temp_graphique>";
  }  
  XML += "</xml>";
}

void page_html()
{
  server.send_P(200, "text/html", SITE_index); // envoie la page principale
}

void page_html_ambiance()
{
  page_html_active= "ambiance";
  server.send_P(200, "text/html", SITE_ambiance); // envoie la page ambiance
  if (server.hasArg("allume")) 
  { 
    bouton_status = 0; // bouton 'Allumer' désactivé, bouton 'Eteindre' activé
    bandeauled.no_animation();
    animation_html = "Aucune";
    bandeauled.ColorSet(couleur(sliderValue[0], sliderValue[1], sliderValue[2]));
    handleXML();
  }
  if (server.hasArg("eteint"))
  {
    bouton_status = 1; // bouton 'Allumer' activé, bouton 'Eteindre' désactivé
    bandeauled.ColorSet(couleur(0, 0, 0)); // aucun affichage
    handleXML();
  }
  if (server.hasArg("sauve")) // sauvegarde dans l'EEPROM des valeurs RGB
  {
    for (int i = 0; i < 3; ++i) { EEPROM.write(96+i, sliderValue[i]); } // on sauvegarde les valeurs RGB dans l'EEPROM   
    EEPROM.commit(); // on valide l'écriture de l'EEPROM
  }
  if ((server.hasArg("restaure")) && (bouton_status == 0)) // restaure depuis l'EEPROM des valeurs RGB
  {
    for (int i= 96; i < 99; ++i) { sliderValue[i - 96] = EEPROM.read(i); } // Récupère les valeurs RGB
    handleXML();
    bandeauled.ColorSet(couleur(sliderValue[0], sliderValue[1], sliderValue[2]));
  }
  if ((server.hasArg("blanc")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(255, 255, 255));
    sliderValue[0] = 255; sliderValue[1] = 255; sliderValue[2] = 255;
    handleXML(); 
  }
  if ((server.hasArg("rouge")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(255, 0, 0)); 
    sliderValue[0] = 255; sliderValue[1] = 0; sliderValue[2] = 0;
    handleXML(); 
  }
  if ((server.hasArg("orange")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(255, 65, 0)); 
    sliderValue[0] = 255; sliderValue[1] = 65; sliderValue[2] = 0;
    handleXML();
  }
  if ((server.hasArg("saumon")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(100, 20, 0)); 
    sliderValue[0] = 100; sliderValue[1] = 20; sliderValue[2] = 0;
    handleXML();
  }
  if ((server.hasArg("jaune")) && (bouton_status == 0)) 
  { 
    bandeauled.ColorSet(couleur(255, 145, 0)); 
    sliderValue[0] = 255; sliderValue[1] = 145; sliderValue[2] = 0;
    handleXML();
  }
  if ((server.hasArg("vert")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(0, 255, 0)); 
    sliderValue[0] = 0; sliderValue[1] = 255; sliderValue[2] = 0;
    handleXML();
  }
  if ((server.hasArg("vertclair")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(60, 120, 20)); 
    sliderValue[0] = 60; sliderValue[1] = 120; sliderValue[2] = 20;
    handleXML();
  }
  if ((server.hasArg("vertmer")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(0, 120, 120)); 
    sliderValue[0] = 0; sliderValue[1] = 120; sliderValue[2] = 120;
    handleXML();
  }
  if ((server.hasArg("bleu")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(0, 0, 255));
    sliderValue[0] = 0; sliderValue[1] = 0; sliderValue[2] = 255;
    handleXML(); 
  }
  if ((server.hasArg("bleuclair")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(20, 120, 255));
    sliderValue[0] = 20; sliderValue[1] = 120; sliderValue[2] = 255;
    handleXML(); 
  }
  if ((server.hasArg("violet")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(100, 0, 60));
    sliderValue[0] = 100; sliderValue[1] = 0; sliderValue[2] = 60;
    handleXML(); 
  }
  if ((server.hasArg("rose")) && (bouton_status == 0))
  { 
    bandeauled.ColorSet(couleur(255, 70, 160)); 
    sliderValue[0] = 255; sliderValue[1] = 70; sliderValue[2] = 160;
    handleXML();
  }
}

void page_html_animations()
{
  page_html_active = "animation";
  server.send_P(200, "text/html", SITE_animations); // envoie la page animations
  bandeauled.setBrightness(100);
  if (server.hasArg("none")) 
  { 
    bouton_status = 1;
    animation_html = "Aucune";
    bandeauled.no_animation();
    bandeauled.ColorSet(couleur(0, 0, 0)); // éteint toutes les LED
  }
  if (server.hasArg("ambiance")) 
  { 
    bouton_status = 1;
    animation_html = "Ambiance";
    bandeauled.ambiance_anime(bandeauled.Color(127, 127, 127), 50, forward); // transition 50ms
  }
  if (server.hasArg("vague")) 
  { 
    bouton_status = 1;
    animation_html = "Vague";
    bandeauled.vague(bandeauled.Color(127, 127, 127), 1); // transition 1ms
  }
  if (server.hasArg("arc_en_ciel")) 
  { 
    bouton_status = 1;
    animation_html = "Arc en ciel";
    bandeauled.arc_en_ciel(10); // transition 10ms
  }
  if (server.hasArg("fondu")) 
  { 
    bouton_status = 1;
    animation_html = "Fondu";
    bandeauled.fondu(bandeauled.Color(0,0,0), bandeauled.Color(255,255,255), 50, 70); // 50 pas, transition 70ms
  }
  if (server.hasArg("theatre")) 
  { 
    bouton_status = 1;
    animation_html = "Theatre";
    bandeauled.theatre(bandeauled.Color(0,0,0), bandeauled.Color(255,255,255), 50); // transition 50ms
  }
  if (server.hasArg("scanner")) 
  { 
    bouton_status = 1;
    animation_html = "Scanner";
    bandeauled.scanner(bandeauled.Color(255,255,255), 50); // transition 50ms
  }
  if (server.hasArg("feu")) 
  { 
    bouton_status = 1;
    animation_html = "Feu";
    bandeauled.feu(bandeauled.Color(255,96,12), 50); // transition 50ms
  }
  if (server.hasArg("volet")) 
  { 
    bouton_status = 1;
    animation_html = "Volet";
    bandeauled.volet(bandeauled.Color(255,96,12), 50, forward); // transition 50ms
  }
  if (server.hasArg("heliport")) 
  { 
    bouton_status = 1;
    animation_html = "Heliport";
    bandeauled.heliport(bandeauled.Color(255,96,12),bandeauled.Color(40,50,125), 50); // transition 50ms
  }
  if (server.hasArg("scintillement")) 
  { 
    bouton_status = 1;
    animation_html = "Scintillement";
    bandeauled.scintillement(bandeauled.Color(255,96,12), 100); // transition 100ms
  }
  if (server.hasArg("halloween")) 
  { 
    bouton_status = 1;
    animation_html = "Halloween";
    bandeauled.halloween(200); // transition 200ms
  }
  if (server.hasArg("france")) 
  { 
    bouton_status = 1;
    animation_html = "France";
    bandeauled.france(50, 1); // transition 200ms étape 1
  }
  if (server.hasArg("meteor")) 
  { 
    bouton_status = 1;
    animation_html = "Meteor";
    bandeauled.meteor(bandeauled.Color(255, 255,255), 1, true, 70); // 1 Led et transition 70ms
  }
  if (server.hasArg("cylon")) 
  { 
    bouton_status = 1;
    animation_html = "Cylon";
    bandeauled.cylon(bandeauled.Color(255, 255,255), 50); // transition 50ms
  }
  handleXML();
}


void page_html_temperature()
{
  page_html_active= "temperature";
  server.send_P(200, "text/html", SITE_temperature); // envoie la page d'affichage température
  handleXML();
}

void page_html_administration()
{
  page_html_active= "administration";
  server.send_P(200, "text/html", SITE_admin); // envoie la page d'administration
  handleXML();
  if (server.hasArg("reboot")) 
  { 
    // redémarre l'ESP (ne fonctionne pas si on a téléchargé le programme via le
    // port série, il faut effectuer un redémarrage du module pour corriger ce problème)
    ESP.restart(); 
  } 
  if (server.hasArg("reinit")) // on réinitialise le WiFi comme lors de la première exécution du programme
  {
    ClearEEprom(); // efface l'EEPROM
    WiFi.mode(WIFI_AP); // configure le WiFi en point d'accès
    ssid_apmode = ssid_apmode + "_" + WiFi.macAddress(); // créer un SSID contenant l'adresse MAC du module
    WiFi.softAP(ssid_apmode.c_str(), password_apmode.c_str(), 10); // configure le module en point d'accès en utilisant le canal 10 
    ssid_actif_html = ssid_apmode.c_str();
  }
  if (server.hasArg("ssid") && server.hasArg("passwd") && server.hasArg("conf_wifi")) // on se connecte à un nouveau point d'accès WiFi
  {  
    ssid = server.arg("ssid"); // récupère le SSID
    password =  server.arg("passwd"); // récupère le mot de passe
    WiFi.mode(WIFI_STA); // configure le WIFI en mode station
    WiFi.disconnect(); // on se déconnecte si on été déjà connecté à un poit d'accès WiFi 
    ClearEEprom(); // efface l'EEPROM
    delay(10);
    for (int i = 0; i < ssid.length(); ++i) { EEPROM.write(i, ssid[i]); } // on saugegarde le SSID dans l'EEPROM
    for (int i = 0; i < password.length(); ++i) { EEPROM.write(32+i, password[i]); } // on sauvegarde le mot de passe dans l'EEPROM   
    EEPROM.commit(); // on valide l'écriture de l'EEPROM
    WiFi.begin(ssid.c_str(), password.c_str()); // connexion au point d'accès WiFi
    while (WiFi.waitForConnectResult() != WL_CONNECTED) // on attend la connexion
    {
      wifi_erreur++; // on incrémente le nbr de tentatives en erreur
      EEPROM.write(99, wifi_erreur); // on enregistre le nbr de tentatives dans l'EEPROM
      EEPROM.commit(); // on valide l'écriture
      delay(5000); // on attend 5 secondes
      ESP.restart(); // il y a une erreur de connexion on reboot
    }
    wifi_erreur = 0; // pas d'erreur
    ssid_actif_html = ssid.c_str();
  }
}

void page_html_non_trouve()
{
  server.send(200, "text/html", "page non existante"); // la page demandée n'existe pas
}

void loop() 
{
  server.handleClient(); // une requête HTTP ?
  ArduinoOTA.handle(); // une requête pour la MAJ via WiFi ?
  if (millis() - compteur >= 900000L) // s'exécute toutes les 15 mn (900000 ms)
  { 
    compteur = millis(); // initialise le compteur
    lecture_temperature();
  }
  bandeauled.Update();
}
